/*
===============================================================================

	c_grid_player.cpp
	Client side implementation of the Grid player entity.

===============================================================================
*/

#ifndef __C_GRID_PLAYER_H__
#define __C_GRID_PLAYER_H__

#include "c_baseplayer.h"
#include "c_grid_base_weapon.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridPlayer : public C_BasePlayer
{
public:
	DECLARE_CLASS( C_GridPlayer, C_BasePlayer );
	DECLARE_CLIENTCLASS();

	C_GridPlayer();

	C_GridBaseWeapon *	GetActiveWeapon()				{ return dynamic_cast<C_GridBaseWeapon *>( _activeWeapon.Get() ); }
	Vector			GetHeadOffset()						{ return _viewoffset; }

private:
	EHANDLE			m_hHand;
	EHANDLE			_activeWeapon;
	Vector			_viewoffset;
};

#endif // __C_GRID_PLAYER_H__