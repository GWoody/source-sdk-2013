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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CHoloHand::CHoloHand()
{
	_activeGestures = 0;

	CBaseEntity *pSprite = CSprite::SpriteCreate( "sprites/glow01.vmt", vec3_origin, true );
	Assert( pSprite );
	_palmSprite.Set( pSprite );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const SFinger &CHoloHand::GetFinger( EFinger finger ) const
{
	Assert( finger >= 0 && finger < NFinger::FINGER_COUNT );
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
void CHoloHand::ProcessFrame( const holo::SFrame &frame )
{
	CBasePlayer *owner = dynamic_cast<CBasePlayer *>( GetOwnerEntity() );
	Assert( owner );

	const Vector dir = owner->BodyDirection2D().Normalized();
	Vector newPos = owner->GetAbsOrigin() + ( dir * frame._hand.palmPosition );

	Msg( "%f %f %f\n", frame._hand.palmPosition.x, frame._hand.palmPosition.y, frame._hand.palmPosition.z );

	SetAbsOrigin( newPos );
	//_palmSprite->SetAbsOrigin( newPos );
}