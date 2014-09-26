/*
===============================================================================

	c_holo_player.cpp
	Client side implementation of the Holodeck player entity.

===============================================================================
*/

#ifndef __C_HOLO_PLAYER_H__
#define __C_HOLO_PLAYER_H__

#include "c_baseplayer.h"
#include "c_holo_haptics.h"
#include "holodeck/holo_shared.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_HoloPlayer : public C_BasePlayer
{
public:
	DECLARE_CLASS( C_HoloPlayer, C_BasePlayer );
	DECLARE_CLIENTCLASS();

	C_HoloPlayer();

	// C_BaseEntity overrides.
	virtual void	Simulate();

	Vector			GetHeadOffset()						{ return _viewoffset; }

private:
	EHANDLE			m_hHand[holo::EHand::HAND_COUNT];
	Vector			_viewoffset;
	C_HoloHaptics	_haptics;
};

#endif // __C_HOLO_PLAYER_H__