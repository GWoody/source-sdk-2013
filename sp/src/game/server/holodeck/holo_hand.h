/*
===============================================================================

	holo_hand.cpp
		Implements the hand entity.
		The hand entity mirrors the client side Leap Motion hand state.
		It is used to test holodeck triggers.

===============================================================================
*/

#ifndef __HOLO_HAND_H__
#define __HOLO_HAND_H__

#include "baseentity.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CHoloHand : public CBaseEntity
{
	// Data types.
public:
	enum EFinger
	{
		FINGER_THUMB,
		FINGER_POINTER,
		FINGER_MIDDLE,
		FINGER_USELESS,
		FINGER_PINKY,

		FINGER_COUNT
	};

	struct SFinger
	{
		Vector direction;
	};

public:
	DECLARE_CLASS( CHoloHand, CBaseEntity );

	const SFinger &	GetFinger( EFinger finger );

private:
	SFinger			_fingers[FINGER_COUNT];
};

#endif // __HOLO_HAND_H__