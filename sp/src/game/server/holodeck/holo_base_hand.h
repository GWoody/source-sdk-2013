/*
===============================================================================

	holo_base_hand.h
		Implements the hand entity.
		The hand entity mirrors the client side Leap Motion hand state.

===============================================================================
*/

#ifndef __HOLO_BASE_HAND_H__
#define __HOLO_BASE_HAND_H__

#include "baseentity.h"
#include "holodeck/holo_shared.h"
#include "holo_frame_filter.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CBaseHoloHand : public CBaseEntity
{
public:
	DECLARE_CLASS( CBaseHoloHand, CBaseEntity );
					CBaseHoloHand();

	// CBaseEntity overrides.
	virtual void	Spawn();
	virtual void	Precache();
	virtual	bool	CreateVPhysics();

	// Accessors.
	const holo::CFrame &	GetFrame() const;

	// Frame processing.
	void			ProcessFrame( const holo::CFrame &frame );

	void			SetInvisible( bool invisible );

	void			DebugStartTouch();
	void			DebugEndTouch();

private:
	void			RenderDebugHand();

	Vector			GetOriginOffset() const;

	void			ExtrapolateFrame();

	holo::CFrame	_transformedFrame, _untransformedFrame;
	int				_activeGestures;

	holo::CFrameFilter _filter;

	Vector			_lastOriginDelta;
};

#endif // __HOLO_BASE_HAND_H__