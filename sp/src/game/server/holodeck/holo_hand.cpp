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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( holo_hand, CHoloHand );

CHoloHand::CHoloHand()
{
	_activeGestures = 0;

	CBaseEntity *pSprite = CSprite::SpriteCreate( "sprites/glow01.vmt", vec3_origin, true );
	Assert( pSprite );
	_palmSprite.Set( pSprite );
}

//-----------------------------------------------------------------------------
// Updates the state of the hand entity.
//-----------------------------------------------------------------------------
void CHoloHand::ProcessClientString( const CCommand &args )
{
	const char *cmd = args[0];

	if( stricmp( cmd, "hand" ) )
	{
		ProcessHandString( args );
	}
	else if( stricmp( cmd, "circlegesture" ) )
	{
		ProcessCircleGestureString( args );
	}
	else if( stricmp( cmd, "swipegesture" ) )
	{
		ProcessSwipeGestureString( args );
	}
	else if( stricmp( cmd, "keytapgesture" ) )
	{
		ProcessKeyTapGestureString( args );
	}
	else if( stricmp( cmd, "screentapgesture" ) )
	{
		ProcessScrenTapGestureString( args );
	}
	else if( stricmp( cmd, "ballgesture" ) )
	{
		ProcessBallGestureString( args );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const SFinger &CHoloHand::GetFinger( EFinger finger ) const
{
	Assert( finger >= 0 && finger < FINGER_COUNT );
	return _fingers[finger];
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const SHand &CHoloHand::GetHand() const
{
	return _hand;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHand::ProcessHandString( const CCommand &args )
{
	istringstream ss( args.GetCommandString() );
	string temp;

	// Clear command name.
	ss >> temp;

	// Funny things happen if we don't have 5 fingers.
	int fingerCount;
	ss >> fingerCount;
	if( fingerCount == 5 )
	{
		for( int i = 0; i < fingerCount; i++ )
		{
			ss >> _fingers[i];
		}
	}
	else
	{
		// Ignore fingers.
		SFinger tempFinger;
		for( int i = 0; i < fingerCount; i++ )
		{
			ss >> tempFinger;
		}
	}

	ss >> _hand;

	CBaseEntity *owner = GetOwnerEntity();
	Assert( owner );

	// Update the entity position.
	SetAbsOrigin( _hand.palmPosition + owner->GetAbsOrigin() );

	Msg( "owner = ( %f %f %f ), hand = ( %f %f %f )\n", owner->GetAbsOrigin().x, owner->GetAbsOrigin().y, owner->GetAbsOrigin().z, GetAbsOrigin().x, GetAbsOrigin().y, GetAbsOrigin().z );

	// Update the visualisation position.
	_palmSprite->SetAbsOrigin( GetAbsOrigin() );
}

//-----------------------------------------------------------------------------
// [hand_id] [finger_id] [center] [normal] [radius]
//-----------------------------------------------------------------------------
void CHoloHand::ProcessCircleGestureString( const CCommand &args )
{

}

//-----------------------------------------------------------------------------
// [direction] [position] [hand_id] [speed] [start_position]
//-----------------------------------------------------------------------------
void CHoloHand::ProcessSwipeGestureString( const CCommand &args )
{

}

//-----------------------------------------------------------------------------
// [direction] [position] [hand_id] [palm_position] [finger_id]
//-----------------------------------------------------------------------------
void CHoloHand::ProcessKeyTapGestureString( const CCommand &args )
{

}

//-----------------------------------------------------------------------------
// [direction] [position] [hand_id] [palm_position] [finger_id]
//-----------------------------------------------------------------------------
void CHoloHand::ProcessScrenTapGestureString( const CCommand &args )
{

}

//-----------------------------------------------------------------------------
// [hand_id] [sphere_center] [sphere_radius] [palm_position]
//-----------------------------------------------------------------------------
void CHoloHand::ProcessBallGestureString( const CCommand &args )
{

}