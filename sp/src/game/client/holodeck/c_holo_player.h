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

class C_HoloHand;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_HoloPlayer : public C_BasePlayer
{
public:
	DECLARE_CLASS( C_HoloPlayer, C_BasePlayer );
	DECLARE_CLIENTCLASS();

	C_HoloPlayer();

	// Accessors.
	Vector			GetHeadOffset()						{ return _viewoffset; }
	C_HoloHand *	GetHand( EHand hand )				{ return (C_HoloHand *)_hands[hand].Get(); }

	static C_HoloPlayer *	GetLocalPlayer()			{ return (C_HoloPlayer *)BaseClass::GetLocalPlayer(); }

private:
	EHANDLE			_hands[::HAND_COUNT];
	Vector			_viewoffset;
};

#endif // __C_HOLO_PLAYER_H__