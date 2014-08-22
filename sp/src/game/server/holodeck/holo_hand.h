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
	const holo::CFrame &	GetFrame() const;

	// Frame processing.
	void			ProcessFrame( const holo::CFrame &frame );

	void			DebugStartTouch();
	void			DebugEndTouch();

private:
	void			RenderDebugHand();

	Vector			GetOriginOffset() const;

	void			ExtrapolateFrame();

	holo::CFrame	_transformedFrame, _untransformedFrame;
	int				_activeGestures;

	Vector			_lastOriginDelta;
};

#endif // __HOLO_HAND_H__