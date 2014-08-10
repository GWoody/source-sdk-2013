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

	// CBaseEntity overrides.
	virtual void	Spawn();
	virtual void	Precache();
	virtual	bool	CreateVPhysics();

	// Accessors.
	const holo::CFinger &	GetFinger( holo::EFinger finger ) const;
	const holo::CHand &	GetHand() const;
	const holo::CFrame &	GetFrame() const;

	// Frame processing.
	void			ProcessFrame( const holo::CFrame &frame );

	void			DebugStartTouch();
	void			DebugEndTouch();

private:
	void			RenderDebugHand();

	// Frame processing.
	bool			IsValidFrame( const holo::CFrame &frame );

	Vector			GetOriginOffset() const;

	holo::CFrame	_curFrame;
	int				_activeGestures;
};

#endif // __HOLO_HAND_H__