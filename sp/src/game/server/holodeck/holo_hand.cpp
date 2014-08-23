/*
===============================================================================

	holo_hand.cpp
		Implements the hand entity.
		The hand entity mirrors the client side Leap Motion hand state.
		It is used to test holodeck triggers.

===============================================================================
*/

#include "cbase.h"
#include "holo_hand.h"

#include <sstream>

using namespace std;
using namespace holo;

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
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( holo_hand, CHoloHand );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CHoloHand::CHoloHand()
{
	_activeGestures = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::Spawn()
{
	BaseClass::Spawn();

	// Configure the hand to interact with triggers.
	SetMoveType( MOVETYPE_FLY );
	SetSolid( SOLID_VPHYSICS );
	CreateVPhysics();

	// Set the bounding box size.
	const Vector bounds = Vector( 2, 2, 2 );
	SetCollisionGroup( COLLISION_GROUP_PLAYER );
	SetCollisionBounds( -bounds, bounds );

	SetRenderColor( 0, 255, 0 );
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

	if( !frame.IsValid() )
	{
		// Use last frames data.
		_transformedFrame = _untransformedFrame;
	}
	else
	{
		_untransformedFrame = _transformedFrame = frame;
	}

	// Move the hands into the correct position (relative to the player).
	_transformedFrame.ToEntitySpace( owner, GetOriginOffset() );

	// Update the position of the hand entity within the world.
	SetAbsOrigin( _transformedFrame.GetHand().GetPosition() );
	_lastOriginDelta = GetAbsOrigin() - owner->GetAbsOrigin();

	if( holo_render_debug_hand.GetInt() != 0 )
	{
		RenderDebugHand();
	}
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
	SetRenderColor( 0, 0, 255 );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::DebugEndTouch()
{
	SetRenderColor( 0, 255, 0 );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::RenderDebugHand()
{
	if( GetEffects() & EF_NODRAW )
	{
		return;
	}

	const Vector handBounds( 0.25f, 0.25f, 0.25f );
	const Vector fingerBounds( 0.05f, 0.05f, 0.05f );
	const Vector &palmPosition = _transformedFrame.GetHand().GetPosition();
	const float duration = 1.0f / 15.0f;
	const int r = ( 1.0f - _transformedFrame.GetHand().GetConfidence() ) * 255.0f;

	// Draw the palm box.
	//debugoverlay->AddBoxOverlay( palmPosition, -handBounds, handBounds, vec3_angle, m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), 127, duration );

	// Draw all fingers.
	for( int i = 0; i < FINGER_COUNT; i++ )
	{
		const CFinger &finger = _transformedFrame.GetHand().GetFingerByType( (EFinger)i );
		for( int j = 0; j < EBone::BONE_COUNT; j++ )
		{
			const CBone &bone = finger.GetBone( (EBone)j );
			
			debugoverlay->AddLineOverlay( bone.GetPrevJoint(), bone.GetNextJoint(), r, m_clrRender.GetG(), m_clrRender.GetB(), false, duration );
			debugoverlay->AddBoxOverlay( bone.GetNextJoint(), -fingerBounds, fingerBounds, vec3_angle, r, m_clrRender.GetG(), m_clrRender.GetB(), 127, duration );
		}
	}

	if( holo_render_debug_hand.GetInt() == 2 )
	{
		// Draw the ball gesture.
		const CBallGesture &ball = _transformedFrame.GetBallGesture();
		NDebugOverlay::Sphere( ball.GetCenter(), ball.GetRadius(), 0, 0, 255, false, duration );
	}

	if( holo_render_debug_hand.GetInt() == 3 )
	{
		// Draw the bounding box.
		const Vector mins = CollisionProp()->OBBMins();
		const Vector maxs = CollisionProp()->OBBMaxs();
		debugoverlay->AddBoxOverlay( GetAbsOrigin(), mins, maxs, vec3_angle, 255, 255, 255, 63, duration );
	}

	if( holo_render_debug_hand.GetInt() == 4 )
	{
		// Draw the palm normal vector.
		debugoverlay->AddLineOverlayAlpha( palmPosition, palmPosition + _transformedFrame.GetHand().GetNormal(), 255, 0, 0, 127, false, duration );

		// Draw the palm direction vector.
		debugoverlay->AddLineOverlayAlpha( palmPosition, palmPosition + _transformedFrame.GetHand().GetDirection(), 0, 0, 255, 127, false, duration );
	}

	if( holo_render_debug_hand.GetInt() == 5 )
	{
		// Draw velocity.
		debugoverlay->AddLineOverlay( palmPosition, palmPosition + _transformedFrame.GetHand().GetVelocity(), 255, 0, 0, false, duration );
	}

	if( holo_render_debug_hand.GetInt() == 6 )
	{
		// Draw finger directions.
		for( int i = 0; i < FINGER_COUNT; i++ )
		{
			const Vector &tipPosition = _transformedFrame.GetHand().GetFingerByType( (EFinger)i ).GetTipPosition();
			const Vector &direction = _transformedFrame.GetHand().GetFingerByType( (EFinger)i ).GetDirection();
			debugoverlay->AddLineOverlayAlpha( tipPosition, tipPosition + direction, 255, 0, 0, 127, false, duration );
		}
	}
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