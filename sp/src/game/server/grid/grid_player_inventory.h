/*
===============================================================================

	grid_player_inventory.h
	Implements weapon management.

===============================================================================
*/

#ifndef __GRID_PLAYER_INVENTORY_H__
#define __GRID_PLAYER_INVENTORY_H__

class CGridBaseWeapon;
class CGridPlayer;

namespace grid
{
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CInventory
	{
	public:
						CInventory( CGridPlayer *player );

		void			SwapWeapons( CGridBaseWeapon *weapon );
		CGridBaseWeapon *	GetWeapon()						{ return _weapon; }

	private:
		CGridBaseWeapon *	_weapon;
		CGridPlayer *	_player;
	};
}

#endif // __GRID_PLAYER_INVENTORY_H__