/*
===============================================================================

	c_grid_player.cpp
	Client side implementation of the Grid player entity.

===============================================================================
*/

#ifndef __C_GRID_PLAYER_H__
#define __C_GRID_PLAYER_H__

#include "c_grid_base_weapon.h"
#include "holodeck/c_holo_player.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridPlayer : public C_HoloPlayer
{
public:
	DECLARE_CLASS( C_GridPlayer, C_BasePlayer );
	DECLARE_CLIENTCLASS();

	C_GridPlayer();

	C_GridBaseWeapon *	GetActiveWeapon()				{ return dynamic_cast<C_GridBaseWeapon *>( _activeWeapon.Get() ); }

private:
	EHANDLE			_activeWeapon;
	EHANDLE			_prop;
};

#endif // __C_GRID_PLAYER_H__