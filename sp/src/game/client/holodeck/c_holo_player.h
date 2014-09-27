/*
===============================================================================

	c_holo_player.cpp
	Client side implementation of the Holodeck player entity.

===============================================================================
*/

#ifndef __C_HOLO_PLAYER_H__
#define __C_HOLO_PLAYER_H__

#include "c_baseplayer.h"
#include "holodeck/holo_shared.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_HoloPlayer : public C_BasePlayer
{
public:
	DECLARE_CLASS( C_HoloPlayer, C_BasePlayer );
	DECLARE_CLIENTCLASS();

	C_HoloPlayer();

	Vector			GetHeadOffset()						{ return _viewoffset; }

private:
	EHANDLE			_hands[holo::EHand::HAND_COUNT];
	Vector			_viewoffset;
};

#endif // __C_HOLO_PLAYER_H__