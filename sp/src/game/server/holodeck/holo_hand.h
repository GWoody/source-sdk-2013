/*
===============================================================================

	holo_hand.h
		Implements the hand entity.
		The hand entity mirrors the client side Leap Motion hand state.
		It is used to test holodeck triggers.

===============================================================================
*/

#ifndef __HOLO_HAND_H__
#define __HOLO_HAND_H__

#include "baseentity.h"
#include "holodeck/holo_shared.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CHoloHand : public CBaseEntity
{
public:
	DECLARE_CLASS( CHoloHand, CBaseEntity );
					CHoloHand();

	// Accessors.
	const holo::SFinger &	GetFinger( holo::EFinger finger ) const;
	const holo::SHand &	GetHand() const;

	// Frame processing.
	void			ProcessFrame( const holo::SFrame &frame );

private:
	holo::SFinger	_fingers[holo::EFinger::FINGER_COUNT];
	holo::SHand		_hand;
	int				_activeGestures;

	EHANDLE			_palmSprite;
};

#endif // __HOLO_HAND_H__