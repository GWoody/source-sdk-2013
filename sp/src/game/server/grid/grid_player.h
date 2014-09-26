/*
===============================================================================

	grid_player.h
	Implements functionality specific for the Grid player.

===============================================================================
*/

#ifndef __GRID_PLAYER_H__
#define __GRID_PLAYER_H__

#include "grid_gesture_detector.h"
#include "grid_player_inventory.h"
#include "holodeck/holo_hand.h"
#include "holodeck/holo_haptics.h"
#include "holodeck/holo_player.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridPlayer : public CHoloPlayer
{
public:
	DECLARE_CLASS( CGridPlayer, CHoloPlayer );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	CGridPlayer();

	static CGridPlayer *CreatePlayer( const char *className, edict_t *ed )
	{
		CGridPlayer::s_PlayerEdict = ed;
		return (CGridPlayer*)CreateEntityByName( className );
	}

	// CBasePlayer overrides.
	virtual void	Spawn();

	// Weapon overrides.
	virtual Vector	Weapon_ShootPosition();

	// Frame updates.
	virtual void	PreThink();

	// Accessors.
	grid::CInventory &	GetInventory()					{ return _inventory; }

private:
	void			ProcessFrame( const holo::CFrame &frame );
	void			HandleGunGesture( const holo::CFrame &frame );

	CNetworkHandle( CGridBaseWeapon, _activeWeapon );
	grid::CInventory	_inventory;
	bool			_weaponWasOut;
	int				_weaponHandIdx;
};

#endif // __GRID_PLAYER_H__