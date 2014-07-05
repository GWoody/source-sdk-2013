/*
===============================================================================

	holo_hand.h
		Implements the hand entity.
		The hand entity mirrors the client side Leap Motion hand state.
		It is used to test holodeck triggers.

===============================================================================
*/

#include "cbase.h"
#include "holo_hand.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( holo_hand, CHoloHand );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const CHoloHand::SFinger &CHoloHand::GetFinger( CHoloHand::EFinger finger )
{
	Assert( finger >= 0 && finger < FINGER_COUNT );
	return _fingers[finger];
}