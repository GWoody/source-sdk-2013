/*
===============================================================================

	grid_player_inventory.h
	Implements weapon management.

===============================================================================
*/

#include "cbase.h"
#include "grid_player_inventory.h"
#include "grid_base_weapon.h"
#include "grid_player.h"

using namespace grid;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CInventory::CInventory( CGridPlayer *player )
{
	_player = player;
	_weapon = NULL;
}

//-----------------------------------------------------------------------------
// Exchanges a weapon in the world with the one in the inventory.
//-----------------------------------------------------------------------------
void CInventory::SwapWeapons( CGridBaseWeapon *weapon )
{
	if( _weapon )
	{
		_weapon->Drop();
	}

	_weapon = weapon;
	weapon->Pickup( _player );
	_player->EmitSound( "Player.PickupWeapon" );
}