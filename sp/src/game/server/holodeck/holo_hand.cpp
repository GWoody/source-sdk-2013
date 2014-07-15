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
#include "Sprite.h"

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
	const Vector bounds = Vector( 16, 16, 16 );
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
void CHoloHand::ProcessFrame( const holo::SFrame &frame )
{
	if( !IsValidFrame( frame ) )
	{
		return;
	}

	CBasePlayer *owner = dynamic_cast<CBasePlayer *>( GetOwnerEntity() );
	Assert( owner );

	// Raise the hand position to be in line with the players virtual arm.
	Vector palmPosition = frame._hand.palmPosition + Vector( 0, 0, 32 );

	Vector dir = owner->BodyDirection2D().Normalized();
	dir.z = 1.0f;
	Vector newPos = owner->GetAbsOrigin() + ( dir * palmPosition );

	SetAbsOrigin( newPos );

	if( holo_render_debug_hand.GetBool() )
	{
		RenderDebugHand();
	}

	_curFrame = frame;
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
	Vector origin = GetAbsOrigin();
	
	debugoverlay->AddBoxOverlay( origin, -handBounds, handBounds, QAngle( 0, 0, 0 ), m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), 127, 1.0f / 30.0f );

	CBasePlayer *owner = dynamic_cast<CBasePlayer *>( GetOwnerEntity() );
	Assert( owner );

	for( int i = 0; i < NFinger::FINGER_COUNT; i++ )
	{
		Vector dir = owner->BodyDirection2D().Normalized();
		dir.z = 1.0f;

		Vector tipPosition = _curFrame._hand.fingers[i].tipPosition + Vector( 0, 0, 32 );
		origin = owner->GetAbsOrigin() + ( dir * tipPosition );

		debugoverlay->AddBoxOverlay( origin, -fingerBounds, fingerBounds, QAngle( 0, 0, 0 ), m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), 127, 1.0f / 30.0f );
		debugoverlay->AddLineOverlayAlpha( GetAbsOrigin(), origin, m_clrRender.GetR(), m_clrRender.GetG(), m_clrRender.GetB(), 127, true, 1.0f / 30.0f );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CHoloHand::IsValidFrame( const holo::SFrame &frame )
{
	return true;
}