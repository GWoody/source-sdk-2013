//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//
#include "cbase.h"
#include "model_types.h"
#include "vcollide.h"
#include "vcollide_parse.h"
#include "solidsetdefaults.h"
#include "bone_setup.h"
#include "engine/ivmodelinfo.h"
#include "physics.h"
#include "c_breakableprop.h"
#include "view.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT(C_BreakableProp, DT_BreakableProp, CBreakableProp)
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_BreakableProp::C_BreakableProp( void )
{
	m_takedamage = DAMAGE_YES;

#ifdef HOLODECK
	_glowAlpha = 0.0f;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BreakableProp::SetFadeMinMax( float fademin, float fademax )
{
	m_fadeMinDist = fademin;
	m_fadeMaxDist = fademax;
}

//-----------------------------------------------------------------------------
// Copy fade from another breakable prop
//-----------------------------------------------------------------------------
void C_BreakableProp::CopyFadeFrom( C_BreakableProp *pSource )
{
	m_flFadeScale = pSource->m_flFadeScale;
}

#ifdef HOLODECK
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_BreakableProp::Simulate()
{
	if( CGlowObject *effect = GetGlowObject() )
	{
		effect->SetAlpha( _glowAlpha * 0.66f );
	}

	BaseClass::Simulate();
}

//-----------------------------------------------------------------------------
// This method is called when the glow effect is rendered.
//-----------------------------------------------------------------------------
void C_BreakableProp::GetGlowEffectColor( float *r, float *g, float *b )
{
	C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();
	if( !player )
	{
		return;
	}

	Vector direction = player->GetAbsOrigin() - GetAbsOrigin();
	float distance = direction.Length();

	const float MIN_PICKUP_DISTANCE = 96.0f;	// As defined in `CBasePlayer::PlayerUse`.
	const float MAX_PICKUP_DISTANCE = 384.0f;
	const float MAX_VISIBLE_DISTANCE = 640.0f;

	// The object is close enough to be picked up by the player.
	// Make it bright green.
	if( distance <= MIN_PICKUP_DISTANCE )
	{
		*r = *b = 0.0f;
		*g = 1.0f;
		_glowAlpha = 1.0f;
		return;
	}

	// The player is close enough to the player for them to think they can pick it up.
	// Fade from red to green to let the player know when they are within range.
	if( distance <= MAX_PICKUP_DISTANCE )
	{
		float delta = MAX_PICKUP_DISTANCE - distance;
		float color = delta / ( MAX_PICKUP_DISTANCE - MIN_PICKUP_DISTANCE );
		*r = 1.0f - color;
		*g = color;
		*b = 0.0f;
		_glowAlpha = 1.0f;
		return;
	}

	// The object is close enough to the player for it to be highlighted.
	// Make it red, but fade transparency.
	if( distance <= MAX_VISIBLE_DISTANCE )
	{
		float delta = MAX_VISIBLE_DISTANCE - distance;
		float color = delta / ( MAX_VISIBLE_DISTANCE - MAX_PICKUP_DISTANCE );
		*r = 1.0f;
		*g = 0.0f;
		*b = 0.0f;
		_glowAlpha = color;
		return;
	}

	// The object is too far away to be of any use to the player.
	// Don't color it.
	*r = 1.0f;
	*g = *b = 0.0f;
	_glowAlpha = 0.0f;
}
#endif