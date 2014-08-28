/*
===============================================================================

	base_fx_impacts.cpp
	Borrowed from `fx_hl2_impacts.cpp`.

===============================================================================
*/

#include "cbase.h"
#include "fx_impact.h"
#include "fx_quad.h"
#include "fx_staticline.h"
#include "fx.h"
#include "tier0/vprof.h"
#include "clienteffectprecachesystem.h"

CLIENTEFFECT_REGISTER_BEGIN( PrecacheTracers )
	CLIENTEFFECT_MATERIAL( "effects/gunshiptracer" )
CLIENTEFFECT_REGISTER_END()

//-----------------------------------------------------------------------------
// Purpose: Handle weapon impacts
//-----------------------------------------------------------------------------
void ImpactCallback( const CEffectData &data )
{
	VPROF_BUDGET( "ImpactCallback", VPROF_BUDGETGROUP_PARTICLE_RENDERING );

	trace_t tr;
	Vector vecOrigin, vecStart, vecShotDir;
	int iMaterial, iDamageType, iHitbox;
	short nSurfaceProp;
	C_BaseEntity *pEntity = ParseImpactData( data, &vecOrigin, &vecStart, &vecShotDir, nSurfaceProp, iMaterial, iDamageType, iHitbox );

	if ( !pEntity )
	{
		// This happens for impacts that occur on an object that's then destroyed.
		// Clear out the fraction so it uses the server's data
		tr.fraction = 1.0;
		PlayImpactSound( pEntity, tr, vecOrigin, nSurfaceProp );
		return;
	}

	// If we hit, perform our custom effects and play the sound
	if ( Impact( vecOrigin, vecStart, iMaterial, iDamageType, iHitbox, pEntity, tr ) )
	{
		// Check for custom effects based on the Decal index
		PerformCustomEffects( vecOrigin, tr, vecShotDir, iMaterial, 1.0 );
	}

	PlayImpactSound( pEntity, tr, vecOrigin, nSurfaceProp );
}

DECLARE_CLIENT_EFFECT( "Impact", ImpactCallback );

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &data - 
//-----------------------------------------------------------------------------
void AR2ImpactCallback( const CEffectData &data )
{
	FX_AddQuad( data.m_vOrigin, 
				data.m_vNormal, 
				random->RandomFloat( 24, 32 ),
				0,
				0.75f, 
				1.0f,
				0.0f,
				0.4f,
				random->RandomInt( 0, 360 ), 
				0,
				Vector( 1.0f, 1.0f, 1.0f ), 
				0.25f, 
				"effects/combinemuzzle2_nocull",
				(FXQUAD_BIAS_SCALE|FXQUAD_BIAS_ALPHA) );
}

DECLARE_CLIENT_EFFECT( "AR2Impact", AR2ImpactCallback );

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : start - 
//			end - 
//			velocity - 
//			makeWhiz - 
//-----------------------------------------------------------------------------
void FX_AR2Tracer( Vector& start, Vector& end, int velocity, bool makeWhiz )
{
	VPROF_BUDGET( "FX_AR2Tracer", VPROF_BUDGETGROUP_PARTICLE_RENDERING );
	
	//Don't make small tracers
	float dist;
	Vector dir;

	VectorSubtract( end, start, dir );
	dist = VectorNormalize( dir );

	// Don't make short tracers.
	if ( dist < 128 )
		return;

	float length = random->RandomFloat( 128.0f, 256.0f );
	float life = ( dist + length ) / velocity;	//NOTENOTE: We want the tail to finish its run as well
	
	//Add it
	FX_AddDiscreetLine( start, dir, velocity, length, dist, random->RandomFloat( 0.5f, 1.5f ), life, "effects/gunshiptracer" );

	if( makeWhiz )
	{
		FX_TracerSound( start, end, TRACER_TYPE_GUNSHIP );	
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : start - 
//			end - 
//-----------------------------------------------------------------------------
void FX_PlayerAR2Tracer( const Vector &start, const Vector &end )
{
	VPROF_BUDGET( "FX_PlayerAR2Tracer", VPROF_BUDGETGROUP_PARTICLE_RENDERING );
	
	Vector	shotDir, dStart, dEnd;
	float	length;

	//Find the direction of the tracer
	VectorSubtract( end, start, shotDir );
	length = VectorNormalize( shotDir );

	//We don't want to draw them if they're too close to us
	if ( length < 128 )
		return;

	//Randomly place the tracer along this line, with a random length
	VectorMA( start, random->RandomFloat( 0.0f, 8.0f ), shotDir, dStart );
	VectorMA( dStart, MIN( length, random->RandomFloat( 256.0f, 1024.0f ) ), shotDir, dEnd );

	//Create the line
	CFXStaticLine *tracerLine = new CFXStaticLine( "Tracer", dStart, dEnd, random->RandomFloat( 6.0f, 12.0f ), 0.01f, "effects/gunshiptracer", 0 );
	assert( tracerLine );

	//Throw it into the list
	clienteffects->AddEffect( tracerLine );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void AR2TracerCallback( const CEffectData &data )
{
	C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();
	
	if ( player == NULL )
		return;

	// Grab the data
	extern Vector GetTracerOrigin( const CEffectData &data );
	Vector vecStart = GetTracerOrigin( data );
	float flVelocity = data.m_flScale;
	bool bWhiz = (data.m_fFlags & TRACER_FLAG_WHIZ);
	int iEntIndex = data.entindex();

	if ( iEntIndex && iEntIndex == player->index )
	{
		Vector	foo = data.m_vStart;
		QAngle	vangles;
		Vector	vforward, vright, vup;

		engine->GetViewAngles( vangles );
		AngleVectors( vangles, &vforward, &vright, &vup );

		VectorMA( data.m_vStart, 4, vright, foo );
		foo[2] -= 0.5f;

		FX_PlayerAR2Tracer( foo, (Vector&)data.m_vOrigin );
		return;
	}
	
	// Use default velocity if none specified
	if ( !flVelocity )
	{
		flVelocity = 8000;
	}

	// Do tracer effect
	FX_AR2Tracer( (Vector&)vecStart, (Vector&)data.m_vOrigin, flVelocity, bWhiz );
}

DECLARE_CLIENT_EFFECT( "AR2Tracer", AR2TracerCallback );