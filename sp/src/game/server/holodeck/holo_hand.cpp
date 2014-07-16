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

static ConVar holo_render_debug_hand( "holo_render_debug_hand", "1" );

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
	const Vector bounds = GetBoundingBox();
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
	Assert( finger >= 0 && finger < NFinger::FINGER_COUNT );
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
void CHoloHand::ProcessFrame( const SFrame &frame )
{
	if( !IsValidFrame( frame ) )
	{
		return;
	}

	// Move the hands into the correct position (relative to the player).
	SFrame processedFrame = frame;
	ApplyVectorRotations( processedFrame );
	ApplyVectorOffsets( processedFrame );

	// Save the frame for future use.
	_curFrame = processedFrame;

	// Update the position of the hand entity within the world.
	SetAbsOrigin( processedFrame._hand.palmPosition );

	if( holo_render_debug_hand.GetBool() )
	{
		RenderDebugHand();
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::DebugStartTouch()
{
#ifdef DEBUG
	SetRenderColor( 255, 0, 0 );
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::DebugEndTouch()
{
#ifdef DEBUG
	SetRenderColor( 0, 255, 0 );
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::RenderDebugHand()
{
	const Vector handBounds( 0.25f, 0.25f, 0.25f );
	const Vector fingerBounds( 0.1f, 0.1f, 0.1f );
	const Vector &palmPosition = _curFrame._hand.palmPosition;
	const float duration = 1.0f / 15.0f;

	// Draw the palm box.
	debugoverlay->AddBoxOverlay( palmPosition, -handBounds, handBounds, vec3_angle, m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), 127, duration );

	const Vector bounds = GetBoundingBox();
	debugoverlay->AddBoxOverlay( GetAbsOrigin(), -bounds, bounds, vec3_angle, 255, 255, 255, 63, duration );

	for( int i = 0; i < NFinger::FINGER_COUNT; i++ )
	{
		const Vector &tipPosition = _curFrame._hand.fingers[i].tipPosition;

		// Draw the finger tip box.
		debugoverlay->AddBoxOverlay( tipPosition, -fingerBounds, fingerBounds, vec3_angle, m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), 127, duration );

		// Draw the finger bone.
		debugoverlay->AddLineOverlayAlpha( palmPosition, tipPosition, m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), 127, true, 1.0f / 30.0f );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CHoloHand::IsValidFrame( const holo::SFrame &frame )
{
	if( frame._hand.palmPosition.IsZero() )
	{
		// The Leap will periodically send frames with (0,0,0) as the position.
		// These should be ignored because they make the hand entity's position
		// flucuate rapidly.
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Move the frame vectors into the correct position (relative to the player).
//-----------------------------------------------------------------------------
void CHoloHand::ApplyVectorOffsets( SFrame &frame )
{
	CBaseEntity *owner = GetOwnerEntity();
	Assert( owner );

	const Vector offset = GetOriginOffset() + owner->GetAbsOrigin();

	frame._ball.center += offset;
	frame._circle.center += offset;
	frame._hand.palmPosition += offset;
	frame._swipe.curPosition += offset;
	frame._swipe.startPosition += offset;
	frame._tap.position += offset;

	for( int i = 0; i < NFinger::FINGER_COUNT; i++ )
	{
		frame._hand.fingers[i].tipPosition += offset;
	}
}

//-----------------------------------------------------------------------------
// Rotate the frame vectors to match the players direction.
//-----------------------------------------------------------------------------
void CHoloHand::ApplyVectorRotations( holo::SFrame &frame )
{
	// Just a macro to make rotating a vector and storing the result back into the vector simple.
	#define VectorYawRotateTemp( vec, angle )		{ Vector temp;	\
		VectorYawRotate( (vec), (angle), temp );					\
		(vec) = temp;												\
	}

	CBasePlayer *owner = dynamic_cast<CBasePlayer *>( GetOwnerEntity() );
	Assert( owner );

	// Convert the players direction vector to angles.
	QAngle ownerAngles;
	VectorAngles( owner->BodyDirection2D(), ownerAngles );

	const float yawAngle = ownerAngles.y;

	// We only need to rotate the forward and side components of the vector.
	// Height can be left alone.
	VectorYawRotateTemp( frame._ball.center, yawAngle );
	VectorYawRotateTemp( frame._circle.center, yawAngle );
	VectorYawRotateTemp( frame._hand.palmPosition, yawAngle );
	VectorYawRotateTemp( frame._swipe.curPosition, yawAngle );
	VectorYawRotateTemp( frame._swipe.startPosition, yawAngle );
	VectorYawRotateTemp( frame._tap.position, yawAngle );

	for( int i = 0; i < NFinger::FINGER_COUNT; i++ )
	{
		VectorYawRotateTemp( frame._hand.fingers[i].tipPosition, yawAngle );
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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Vector CHoloHand::GetBoundingBox() const
{
	return Vector( 4, 4, 4 );
}