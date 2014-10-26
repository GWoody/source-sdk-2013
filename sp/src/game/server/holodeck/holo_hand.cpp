/*
===============================================================================

	holo_hand.cpp
		Implements the hand entity.
		The hand entity mirrors the client side Leap Motion hand state.

===============================================================================
*/

#include "cbase.h"
#include "holo_hand.h"
#include "player_pickup.h"
#include "weapon_physcannon.h"
#include "base_pickupcontroller.h"
#include "ai_basenpc.h"

using namespace holo;

//-----------------------------------------------------------------------------
// ConVars.
//-----------------------------------------------------------------------------
extern ConVar sv_debug_player_use;
static ConVar holo_pickup_cooldown( "holo_pickup_cooldown", "1", FCVAR_ARCHIVE, "Length of time after picking up or dropping an object before being able to pickup or drop an object again." );

//-----------------------------------------------------------------------------
// External functions.
//-----------------------------------------------------------------------------
extern bool PlayerPickupControllerIsHoldingEntity( CBaseEntity *pPickupController, CBaseEntity *pHeldEntity );
extern float PlayerPickupGetHeldObjectMass( CBaseEntity *pPickupControllerEntity, IPhysicsObject *pHeldObject );
extern void PlayerPickupObject( CBasePlayer *pPlayer, CBaseEntity *pObject );

static const char *holo_render_debug_hand_options = "Rendering options:\n\
0 - off,\n\
1 - wireframe hand,\n\
2 - ball gesture,\n\
3 - collision box,\n\
4 - palm direction + normal.\n\
5 - velocity vector.\n\
6 - finger direction vectors.\n\
";

static ConVar holo_render_debug_hand( "holo_render_debug_hand", "1", NULL, holo_render_debug_hand_options );

//-----------------------------------------------------------------------------
// Source entity configuration.
//-----------------------------------------------------------------------------

// Define fields.
BEGIN_DATADESC( CHoloHand )

	DEFINE_EMBEDDED( _haptics ),

END_DATADESC()

// Networking table.
IMPLEMENT_SERVERCLASS_ST( CHoloHand, DT_HoloHand )

	SendPropDataTable( SENDINFO_DT(_haptics), &REFERENCE_SEND_TABLE(DT_HoloHaptics), SendProxy_SendLocalDataTable ),

END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( holo_hand, CHoloHand );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CHoloHand::CHoloHand()
{
	_nextPickupTime = 0.0f;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::Spawn()
{
	_nextPickupTime = 0.0f;

	BaseClass::Spawn();

	// Configure the hand to interact with triggers.
	SetMoveType( MOVETYPE_FLY );
	SetSolid( SOLID_VPHYSICS );
	CreateVPhysics();

	// Set the bounding box size.
	const Vector bounds = Vector( 2, 2, 2 );
	SetCollisionGroup( COLLISION_GROUP_PLAYER );
	SetCollisionBounds( -bounds, bounds );

	SetRenderColor( 192, 192, 192 );

	SetNextThink( gpGlobals->curtime + 0.01f ); 
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::Precache()
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CHoloHand::CreateVPhysics()
{
	VPhysicsInitShadow( false, false );
	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::Think()
{
	_haptics.Update();
	NetworkStateChanged();

	if( _heldEntity )
	{
		_heldEntity->Use( this, this, USE_SET, 2 );
	}

	BaseClass::Think();
	SetNextThink( gpGlobals->curtime + 0.01f ); 
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int CHoloHand::UpdateTransmitState()
{
	// Always transmit this entity to all clients.
	return SetTransmitState( FL_EDICT_ALWAYS );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::OwnerKilled()
{
	ClearUseEntity();
	_haptics.ClearAllEvents();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::LevelShutdown()
{
	_haptics.ClearAllEvents();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::SetType( EHand type )
{ 
	_type = type; 
	_haptics.SetTargetHand( type ); 
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const CFrame &CHoloHand::GetFrame() const
{
	return _transformedFrame;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::ProcessFrame( const CFrame &frame )
{
	CBasePlayer *owner = dynamic_cast<CBasePlayer *>( GetOwnerEntity() );
	Assert( owner );

	const CFrame *source;
	if( !frame.IsValid() )
	{
		// Use last frames data.
		source = &_untransformedFrame;
	}
	else
	{
		source = &frame;
		_untransformedFrame = frame;
	}

	_haptics.SetActive( true );

	// Apply filtering to the source frame.
	_transformedFrame = _filter.FilterFrame( *source );

	// Move the hands into the correct position (relative to the player).
	_transformedFrame.ToEntitySpace( owner, GetOriginOffset() );

	// Update the position of the hand entity within the world.
	SetAbsOrigin( _transformedFrame.GetHand(_type).GetPosition() );

	if( holo_render_debug_hand.GetInt() != 0 )
	{
		RenderDebugHand();
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::OnInvalidFrame()
{
	ClearUseEntity();
	_haptics.SetActive( false );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::SetInvisible( bool invisible )
{
	if( invisible )
	{
		AddEffects( EF_NODRAW );
	}
	else
	{
		RemoveEffects( EF_NODRAW );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::DebugStartTouch()
{
#ifdef DEBUG
	SetRenderColor( 0, 0, 255 );
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::DebugEndTouch()
{
#ifdef DEBUG
	SetRenderColor( 255, 255, 255 );
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::RenderDebugHand()
{
	if( GetEffects() & EF_NODRAW )
	{
		return;
	}

	const float duration = 1.0f / 15.0f;

	DrawHandOutline( duration );
	DrawFingers( duration );
	DrawArm( duration );
	DrawSpecial( duration );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::DrawFingers( float duration )
{
	EFinger circleFinger = _transformedFrame.GetCircleGesture().GetFingerType();

	for( int i = 0; i < FINGER_COUNT; i++ )
	{
		const CFinger &finger = GetHoloHand().GetFingerByType( (EFinger)i );
		
		for( int j = 1; j < EBone::BONE_COUNT; j++ )
		{
			const CBone &bone = finger.GetBone( (EBone)j );

			int red = ( circleFinger == i ) ? 0 : m_clrRender.GetR();
			int green = ( circleFinger == i ) ? 255 : m_clrRender.GetG();
			int blue = ( circleFinger == i ) ? 0 : m_clrRender.GetB();

			if( i == FINGER_THUMB && j == BONE_PROXIMAL )
			{
				// Drawing the proximal on the thumb looks...weird.
				debugoverlay->AddLineOverlay( bone.GetNextJoint(), bone.GetPrevJoint(), red, green, blue, false, duration );
				continue;
			}

			float width = finger.GetWidth() / 8;

#if 0
			float halfWidth = width / 2;

			// Don't know why, but the angles generated are off by 90 degrees.
			QAngle angles;
			VectorAngles( bone.GetDirection(), angles );
			angles += QAngle( 90, 0, 0 );

			// Generate the bounding volume of the fingers.
			float length = ( bone.GetNextJoint() - bone.GetPrevJoint() ).Length();
			Vector maxs( halfWidth, halfWidth, length );
			Vector mins( -halfWidth, -halfWidth, 0 );

			debugoverlay->AddBoxOverlay( bone.GetPrevJoint(), mins, maxs, angles, red, green, blue, 127, duration );
			debugoverlay->AddBoxOverlay( bone.GetNextJoint(), -Vector(width), Vector(width), angles, red, green, blue, 127, duration );
#else
			debugoverlay->AddBoxOverlay( bone.GetNextJoint(), -Vector(width), Vector(width), vec3_angle, red, green, blue, 127, duration );
			debugoverlay->AddBoxOverlay( bone.GetPrevJoint(), -Vector(width), Vector(width), vec3_angle, red, green, blue, 127, duration );
			debugoverlay->AddLineOverlay( bone.GetPrevJoint(), bone.GetNextJoint(), red, green, blue, false, duration );
#endif
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::DrawArm( float duration )
{
	const CArm &arm = GetHoloHand().GetArm();
	debugoverlay->AddLineOverlay( arm.GetWristPosition(), arm.GetElbowPosition(), m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), false, duration );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::DrawSpecial( float duration )
{
	const CHand &hand = GetHoloHand();
	const Vector bounds( 0.05f, 0.05f, 0.05f );
	const Vector &palmPosition = hand.GetPosition();

	if( holo_render_debug_hand.GetInt() == 2 )
	{
		// Draw the ball gesture.
		const CBallGesture &ball = hand.GetBallGesture();
		NDebugOverlay::Sphere( ball.GetCenter(), ball.GetRadius(), 0, 0, 255, false, duration );
	}
	else if( holo_render_debug_hand.GetInt() == 3 )
	{
		// Draw the bounding box.
		const Vector mins = CollisionProp()->OBBMins();
		const Vector maxs = CollisionProp()->OBBMaxs();
		debugoverlay->AddBoxOverlay( GetAbsOrigin(), mins, maxs, vec3_angle, 255, 255, 255, 63, duration );
	}
	else if( holo_render_debug_hand.GetInt() == 4 )
	{
		// Draw the palm normal vector.
		debugoverlay->AddLineOverlayAlpha( palmPosition, palmPosition + hand.GetNormal(), 255, 0, 0, 127, false, duration );

		// Draw the palm direction vector.
		debugoverlay->AddLineOverlayAlpha( palmPosition, palmPosition + hand.GetDirection(), 0, 0, 255, 127, false, duration );
	}
	else if( holo_render_debug_hand.GetInt() == 5 )
	{
		// Draw velocity.
		debugoverlay->AddLineOverlay( palmPosition, palmPosition + hand.GetVelocity(), 255, 0, 0, false, duration );
	}
	else if( holo_render_debug_hand.GetInt() == 6 )
	{
		// Draw finger directions.
		for( int i = 0; i < FINGER_COUNT; i++ )
		{
			const Vector &tipPosition = hand.GetFingerByType( (EFinger)i ).GetTipPosition();
			const Vector &direction = hand.GetFingerByType( (EFinger)i ).GetDirection();
			debugoverlay->AddLineOverlayAlpha( tipPosition, tipPosition + direction, 255, 0, 0, 127, false, duration );
		}
	}
	else if( holo_render_debug_hand.GetInt() == 7 )
	{
		// Draw finger tips.
		for( int i = 0; i < FINGER_COUNT; i++ )
		{
			const Vector &tipPosition = hand.GetFingerByType( (EFinger)i ).GetTipPosition();
			debugoverlay->AddBoxOverlay( tipPosition, -bounds * 2, bounds * 2, vec3_angle, 255, 0, 0, 255, duration );
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::DrawHandOutline( float duration )
{
	const CHand &hand = GetHoloHand();

	const CFinger &thumb = hand.GetFingerByType( EFinger::FINGER_THUMB );
	const CFinger &pointer = hand.GetFingerByType( EFinger::FINGER_POINTER );
	const CFinger &middle = hand.GetFingerByType( EFinger::FINGER_MIDDLE );
	const CFinger &ring = hand.GetFingerByType( EFinger::FINGER_RING );
	const CFinger &pinky = hand.GetFingerByType( EFinger::FINGER_PINKY );

	const CBone thumbBone = thumb.GetBone( EBone::BONE_METACARPAL );
	const CBone pointerBone = pointer.GetBone( EBone::BONE_METACARPAL );
	const CBone middleBone = middle.GetBone( EBone::BONE_METACARPAL );
	const CBone ringBone = ring.GetBone( EBone::BONE_METACARPAL );
	const CBone pinkyBone = pinky.GetBone( EBone::BONE_METACARPAL );

	const CBone thumbProximal = thumb.GetBone( EBone::BONE_PROXIMAL );

	const CArm &arm = hand.GetArm();

	// Web the fingers.
	debugoverlay->AddLineOverlay( thumbProximal.GetNextJoint(), pointerBone.GetNextJoint(), m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), false, duration );
	debugoverlay->AddLineOverlay( pointerBone.GetNextJoint(), middleBone.GetNextJoint(), m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), false, duration );
	debugoverlay->AddLineOverlay( middleBone.GetNextJoint(), ringBone.GetNextJoint(), m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), false, duration );
	debugoverlay->AddLineOverlay( ringBone.GetNextJoint(), pinkyBone.GetNextJoint(), m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), false, duration );

	// Link thumb and pinky to the arm.
	debugoverlay->AddLineOverlay( arm.GetWristPosition(), pinkyBone.GetNextJoint(), m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), false, duration );
	debugoverlay->AddLineOverlay( arm.GetWristPosition(), thumbBone.GetNextJoint(), m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), false, duration );
	debugoverlay->AddLineOverlay( thumbBone.GetNextJoint() + arm.GetDirection() * arm.GetLength(), thumbBone.GetNextJoint(), m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), false, duration );

	// Draw joints.
	const Vector fingerBounds( 0.05f, 0.05f, 0.05f );
	debugoverlay->AddBoxOverlay( thumbBone.GetNextJoint(), -fingerBounds, fingerBounds, vec3_angle, m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), 127, duration );
	debugoverlay->AddBoxOverlay( pointerBone.GetNextJoint(), -fingerBounds, fingerBounds, vec3_angle, m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), 127, duration );
	debugoverlay->AddBoxOverlay( middleBone.GetNextJoint(), -fingerBounds, fingerBounds, vec3_angle, m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), 127, duration );
	debugoverlay->AddBoxOverlay( middleBone.GetNextJoint(), -fingerBounds, fingerBounds, vec3_angle, m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), 127, duration );
	debugoverlay->AddBoxOverlay( ringBone.GetNextJoint(), -fingerBounds, fingerBounds, vec3_angle, m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), 127, duration );
	debugoverlay->AddBoxOverlay( arm.GetWristPosition(), -fingerBounds, fingerBounds, vec3_angle, m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), 127, duration );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const CHand &CHoloHand::GetHoloHand() const
{
	return _transformedFrame.GetHand( _type );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::SetUseEntity( CBaseEntity *entity )
{
	_heldEntity.Set( entity );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CBaseEntity *CHoloHand::GetUseEntity()
{
	return _heldEntity.Get();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CHoloHand::ClearUseEntity()
{
	if ( _heldEntity != NULL )
	{
		_heldEntity->Use( this, this, USE_OFF, 0 );
		_heldEntity = NULL;
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Modified from `CBasePlayer::FindUseEntity` for the hand entity.
//-----------------------------------------------------------------------------
CBaseEntity *CHoloHand::FindUseEntity()
{
	// Attempt to pick up objects that are occluded by the hand.
	Vector forward = GetAbsOrigin() - GetOwnerPlayer()->EyePosition();
	Vector up( 0, 0, 1 );

	forward.NormalizeInPlace();

	trace_t tr;
	// Search for objects in a sphere (tests for entities that are not solid, yet still useable)
	Vector searchCenter = GetAbsOrigin();

	// NOTE: Some debris objects are useable too, so hit those as well
	// A button, etc. can be made out of clip brushes, make sure it's +useable via a traceline, too.
	int useableContents = MASK_SOLID | CONTENTS_DEBRIS | CONTENTS_PLAYERCLIP;

	CBaseEntity *pFoundByTrace = NULL;

	// UNDONE: Might be faster to just fold this range into the sphere query
	CBaseEntity *pObject = NULL;

	float nearestDist = FLT_MAX;
	// try the hit entity if there is one, or the ground entity if there isn't.
	CBaseEntity *pNearest = NULL;

	const int NUM_TANGENTS = 8;
	// trace a box at successive angles down
	//							forward, 45 deg, 30 deg, 20 deg, 15 deg, 10 deg, -10, -15
	const float tangents[NUM_TANGENTS] = { 0, 1, 0.57735026919f, 0.3639702342f, 0.267949192431f, 0.1763269807f, -0.1763269807f, -0.267949192431f };
	for ( int i = 0; i < NUM_TANGENTS; i++ )
	{
		if ( i == 0 )
		{
			UTIL_TraceLine( searchCenter, searchCenter + forward * 1024, useableContents, this, COLLISION_GROUP_NONE, &tr );
		}
		else
		{
			Vector down = forward - tangents[i]*up;
			VectorNormalize(down);
			UTIL_TraceHull( searchCenter, searchCenter + down * 72, -Vector(16,16,16), Vector(16,16,16), useableContents, this, COLLISION_GROUP_NONE, &tr );
		}
		pObject = tr.m_pEnt;

		pFoundByTrace = pObject;

		bool bUsable = GetOwnerPlayer()->IsUseableEntity(pObject, 0);
		while ( pObject && !bUsable && pObject->GetMoveParent() )
		{
			pObject = pObject->GetMoveParent();
			bUsable = GetOwnerPlayer()->IsUseableEntity(pObject, 0);
		}

		if ( bUsable )
		{
			Vector delta = tr.endpos - tr.startpos;
			float centerZ = CollisionProp()->WorldSpaceCenter().z;
			delta.z = IntervalDistance( tr.endpos.z, centerZ + CollisionProp()->OBBMins().z, centerZ + CollisionProp()->OBBMaxs().z );
			float dist = delta.Length();
			if ( dist < PLAYER_USE_RADIUS )
			{
				if ( sv_debug_player_use.GetBool() )
				{
					NDebugOverlay::Line( searchCenter, tr.endpos, 0, 255, 0, true, 30 );
					NDebugOverlay::Cross3D( tr.endpos, 16, 0, 255, 0, true, 30 );
				}

				if ( pObject->MyNPCPointer() && pObject->MyNPCPointer()->IsPlayerAlly( GetOwnerPlayer() ) )
				{
					// If about to select an NPC, do a more thorough check to ensure
					// that we're selecting the right one from a group.
					pObject = GetOwnerPlayer()->DoubleCheckUseNPC( pObject, searchCenter, forward );
				}

				if ( sv_debug_player_use.GetBool() )
				{
					Msg( "Trace using: %s\n", pObject ? pObject->GetDebugName() : "no usable entity found" );
				}

				pNearest = pObject;
				
				// if this is directly under the cursor just return it now
				if ( i == 0 )
					return pObject;
			}
		}
	}

	// check ground entity first
	// if you've got a useable ground entity, then shrink the cone of this search to 45 degrees
	// otherwise, search out in a 90 degree cone (hemisphere)
	if ( GetGroundEntity() && GetOwnerPlayer()->IsUseableEntity(GetGroundEntity(), FCAP_USE_ONGROUND) )
	{
		pNearest = GetGroundEntity();
	}
	if ( pNearest )
	{
		// estimate nearest object by distance from the view vector
		Vector point;
		pNearest->CollisionProp()->CalcNearestPoint( searchCenter, &point );
		nearestDist = CalcDistanceToLine( point, searchCenter, forward );
		if ( sv_debug_player_use.GetBool() )
		{
			Msg("Trace found %s, dist %.2f\n", pNearest->GetClassname(), nearestDist );
		}
	}

	for ( CEntitySphereQuery sphere( searchCenter, PLAYER_USE_RADIUS ); ( pObject = sphere.GetCurrentEntity() ) != NULL; sphere.NextEntity() )
	{
		if ( !pObject )
			continue;

		if ( !GetOwnerPlayer()->IsUseableEntity( pObject, FCAP_USE_IN_RADIUS ) )
			continue;

		// see if it's more roughly in front of the player than previous guess
		Vector point;
		pObject->CollisionProp()->CalcNearestPoint( searchCenter, &point );

		Vector dir = point - searchCenter;
		VectorNormalize(dir);
		float dot = DotProduct( dir, forward );

		// Need to be looking at the object more or less
		if ( dot < 0.8 )
			continue;

		float dist = CalcDistanceToLine( point, searchCenter, forward );

		if ( sv_debug_player_use.GetBool() )
		{
			Msg("Radius found %s, dist %.2f\n", pObject->GetClassname(), dist );
		}

		if ( dist < nearestDist )
		{
			// Since this has purely been a radius search to this point, we now
			// make sure the object isn't behind glass or a grate.
			trace_t trCheckOccluded;
			UTIL_TraceLine( searchCenter, point, useableContents, this, COLLISION_GROUP_NONE, &trCheckOccluded );

			if ( trCheckOccluded.fraction == 1.0 || trCheckOccluded.m_pEnt == pObject )
			{
				pNearest = pObject;
				nearestDist = dist;
			}
		}
	}

	if ( !pNearest )
	{
		// Haven't found anything near the player to use, nor any NPC's at distance.
		// Check to see if the player is trying to select an NPC through a rail, fence, or other 'see-though' volume.
		trace_t trAllies;
		UTIL_TraceLine( searchCenter, searchCenter + forward * PLAYER_USE_RADIUS, MASK_OPAQUE_AND_NPCS, this, COLLISION_GROUP_NONE, &trAllies );

		if ( trAllies.m_pEnt && GetOwnerPlayer()->IsUseableEntity( trAllies.m_pEnt, 0 ) && trAllies.m_pEnt->MyNPCPointer() && trAllies.m_pEnt->MyNPCPointer()->IsPlayerAlly( GetOwnerPlayer() ) )
		{
			// This is an NPC, take it!
			pNearest = trAllies.m_pEnt;
		}
	}

	if ( pNearest && pNearest->MyNPCPointer() && pNearest->MyNPCPointer()->IsPlayerAlly( GetOwnerPlayer() ) )
	{
		pNearest = GetOwnerPlayer()->DoubleCheckUseNPC( pNearest, searchCenter, forward );
	}

	if ( sv_debug_player_use.GetBool() )
	{
		if ( !pNearest )
		{
			NDebugOverlay::Line( searchCenter, tr.endpos, 255, 0, 0, true, 30 );
			NDebugOverlay::Cross3D( tr.endpos, 16, 255, 0, 0, true, 30 );
		}
		else if ( pNearest == pFoundByTrace )
		{
			NDebugOverlay::Line( searchCenter, tr.endpos, 0, 255, 0, true, 30 );
			NDebugOverlay::Cross3D( tr.endpos, 16, 0, 255, 0, true, 30 );
		}
		else
		{
			NDebugOverlay::Box( pNearest->WorldSpaceCenter(), Vector(-8, -8, -8), Vector(8, 8, 8), 0, 255, 0, true, 30 );
		}
	}

	if ( sv_debug_player_use.GetBool() )
	{
		Msg( "Radial using: %s\n", pNearest ? pNearest->GetDebugName() : "no usable entity found" );
	}

	return pNearest;
}

//-----------------------------------------------------------------------------
// Borrowed from `baseplayer_shared.cpp`.
//-----------------------------------------------------------------------------
float CHoloHand::IntervalDistance( float x, float x0, float x1 )
{
	// swap so x0 < x1
	if ( x0 > x1 )
	{
		float tmp = x0;
		x0 = x1;
		x1 = tmp;
	}

	if ( x < x0 )
		return x0-x;
	else if ( x > x1 )
		return x - x1;
	return 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CHoloHand::IsHoldingEntity( CBaseEntity *pEnt )
{
	return PlayerPickupControllerIsHoldingEntity( _heldEntity, pEnt );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float CHoloHand::GetHeldObjectMass( IPhysicsObject *pHeldObject )
{
	return PlayerPickupGetHeldObjectMass( _heldEntity, pHeldObject );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::AttemptObjectPickup()
{
	if( _nextPickupTime > gpGlobals->curtime || GetUseEntity() )
	{
		return;
	}

	CBaseEntity *pUseEntity = FindUseEntity();
	if ( pUseEntity )
	{	
		int caps = pUseEntity->ObjectCaps();
		variant_t emptyVariant;
		bool used = false;

		if ( caps & (FCAP_IMPULSE_USE|FCAP_ONOFF_USE) )
		{
			pUseEntity->AcceptInput( "Use", this, this, emptyVariant, USE_TOGGLE );
			used = true;
		}
		else if ( pUseEntity->ObjectCaps() & FCAP_ONOFF_USE )
		{
			pUseEntity->AcceptInput( "Use", this, this, emptyVariant, USE_TOGGLE );
			used = true;
		}

		if( used )
		{
			_nextPickupTime = gpGlobals->curtime + holo_pickup_cooldown.GetFloat();
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::AttemptObjectDrop()
{
	ClearUseEntity();
	_nextPickupTime = gpGlobals->curtime + holo_pickup_cooldown.GetFloat();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::PickupObject( CBaseEntity *pObject, bool bLimitMassAndSize )
{
	// can't pick up what you're standing on
	if ( GetOwnerEntity()->GetGroundEntity() == pObject )
		return;
	
	if ( bLimitMassAndSize == true )
	{
		if ( CanPickupObject( pObject, 35, 128 ) == false )
			 return;
	}

	// Can't be picked up if NPCs are on me
	if ( pObject->HasNPCsOnIt() )
		return;

	//Don't pick up if we don't have a phys object.
	if ( pObject->VPhysicsGetObject() == NULL )
		 return;

	CPlayerPickupController *pController = (CPlayerPickupController *)CBaseEntity::Create( "player_pickup", pObject->GetAbsOrigin(), vec3_angle, GetOwnerPlayer() );
	
	if ( !pController )
		return;

	pController->Init( this, pObject );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CHoloHand::CanPickupObject( CBaseEntity *pObject, float massLimit, float sizeLimit )
{
	//Must be valid
	if ( pObject == NULL )
		return false;

	//Must move with physics
	if ( pObject->GetMoveType() != MOVETYPE_VPHYSICS )
		return false;

	IPhysicsObject *pList[VPHYSICS_MAX_OBJECT_LIST_COUNT];
	int count = pObject->VPhysicsGetObjectList( pList, ARRAYSIZE(pList) );

	//Must have a physics object
	if (!count)
		return false;

	float objectMass = 0;
	bool checkEnable = false;
	for ( int i = 0; i < count; i++ )
	{
		objectMass += pList[i]->GetMass();
		if ( !pList[i]->IsMoveable() )
		{
			checkEnable = true;
		}
		if ( pList[i]->GetGameFlags() & FVPHYSICS_NO_PLAYER_PICKUP )
			return false;
		if ( pList[i]->IsHinged() )
			return false;
	}

	//Must be under our threshold weight
	if ( massLimit > 0 && objectMass > massLimit )
		return false;

	if ( sizeLimit > 0 )
	{
		const Vector &size = pObject->CollisionProp()->OBBSize();
		if ( size.x > sizeLimit || size.y > sizeLimit || size.z > sizeLimit )
			return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Vector CHoloHand::GetOriginOffset() const
{
#if 0
	CBasePlayer *owner = dynamic_cast<CBasePlayer *>( GetOwnerEntity() );
	Assert( owner );

	const Vector mins = owner->GetPlayerMins();
	const Vector maxs = owner->GetPlayerMaxs();

	// Get the height of the players collision volume.
	const float height = fabsf( maxs.z - mins.z );

	// The player origin at the players feet.
	// We need to apply a vertical offset to bring the origin in line with the
	// players shoulder.
	return Vector( 0, 0, height * 0.8f );
#else
	return Vector( 0, 0, 60 );
#endif
}