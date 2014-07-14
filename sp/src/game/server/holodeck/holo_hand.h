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

	// Client state updates.
	void			ProcessClientString( const CCommand &args );

	// Accessors.
	const holo::SFinger &	GetFinger( holo::EFinger::type finger ) const;
	const holo::SHand &	GetHand() const;

private:
	// Client state updates.
	void			ProcessHandString( const CCommand &args );
	void			ProcessCircleGestureString( const CCommand &args );
	void			ProcessSwipeGestureString( const CCommand &args );
	void			ProcessKeyTapGestureString( const CCommand &args );
	void			ProcessScrenTapGestureString( const CCommand &args );
	void			ProcessBallGestureString( const CCommand &args );

	holo::SFinger	_fingers[holo::EFinger::FINGER_COUNT];
	holo::SHand		_hand;
	int				_activeGestures;

	EHANDLE			_palmSprite;
};

#endif // __HOLO_HAND_H__