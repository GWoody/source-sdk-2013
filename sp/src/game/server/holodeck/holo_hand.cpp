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
1 - palm + finger tips + finger directions,\n\
2 - ball gesture,\n\
3 - collision box,\n\
4 - palm direction + normal.\
5 - velocity vector.\
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
const SFinger &CHoloHand::GetFinger( EFinger finger ) const
{
	Assert( finger >= 0 && finger < FINGER_COUNT );
	return _curFrame._hand.fingers[finger];
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const SHand &CHoloHand::GetHand() const
{
	return _curFrame._hand;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const SFrame &CHoloHand::GetFrame() const
{
	return _curFrame;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::ProcessFrame( const SFrame &frame )
{
	if( !IsValidFrame( frame ) )
	{
		return;
	}

	CBasePlayer *owner = dynamic_cast<CBasePlayer *>( GetOwnerEntity() );
	Assert( owner );

	// Move the hands into the correct position (relative to the player).
	SFrame processedFrame = frame;
	processedFrame.ToEntitySpace( owner, GetOriginOffset() );

	// Save the frame for future use.
	_curFrame = processedFrame;

	// Update the position of the hand entity within the world.
	SetAbsOrigin( processedFrame._hand.position );

	if( holo_render_debug_hand.GetInt() != 0 )
	{
		RenderDebugHand();
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::DebugStartTouch()
{
	SetRenderColor( 255, 0, 0 );
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
	const Vector handBounds( 0.25f, 0.25f, 0.25f );
	const Vector fingerBounds( 0.1f, 0.1f, 0.1f );
	const Vector &palmPosition = _curFrame._hand.position;
	const float duration = 1.0f / 15.0f;

	// Draw the palm box.
	debugoverlay->AddBoxOverlay( palmPosition, -handBounds, handBounds, vec3_angle, m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), 127, duration );

	// Draw all fingers.
	for( int i = 0; i < FINGER_COUNT; i++ )
	{
		const Vector &tipPosition = _curFrame._hand.fingers[i].tipPosition;
		const Vector &direction = _curFrame._hand.fingers[i].direction;

		// Draw the finger tip box.
		debugoverlay->AddBoxOverlay( tipPosition, -fingerBounds, fingerBounds, vec3_angle, m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), 127, duration );

		// Draw the finger direction.
		debugoverlay->AddLineOverlayAlpha( tipPosition, tipPosition + direction, m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), 127, false, duration );
	}

	if( holo_render_debug_hand.GetInt() == 2 )
	{
		// Draw the ball gesture.
		const SBallGesture &ball = _curFrame._ball;
		NDebugOverlay::Sphere( ball.center, ball.radius, 0, 0, 255, false, duration );
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
		debugoverlay->AddLineOverlayAlpha( palmPosition, palmPosition + _curFrame._hand.normal, 255, 0, 0, 127, false, duration );

		// Draw the palm direction vector.
		debugoverlay->AddLineOverlayAlpha( palmPosition, palmPosition + _curFrame._hand.direction, 0, 0, 255, 127, false, duration );
	}

	if( holo_render_debug_hand.GetInt() == 5 )
	{
		// Draw velocity.
		debugoverlay->AddLineOverlay( palmPosition, palmPosition + _curFrame._hand.velocity, 255, 0, 0, false, duration );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CHoloHand::IsValidFrame( const holo::SFrame &frame )
{
	if( frame._hand.position.IsZero() )
	{
		// The Leap will periodically send frames with (0,0,0) as the position.
		// These should be ignored because they make the hand entity's position
		// flucuate rapidly.
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