/*
===============================================================================

	grid_player.h
	Implements functionality specific for the Grid player.

===============================================================================
*/

#ifndef __GRID_PLAYER_H__
#define __GRID_PLAYER_H__

#include "base_grid_player.h"
#include "grid_gesture_detector.h"
#include "grid_player_inventory.h"
#include "holodeck/holo_hand.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridPlayer : public CBaseGridPlayer
{
public:
	DECLARE_CLASS( CGridPlayer, CBaseGridPlayer );
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
	virtual bool	BumpWeapon( CBaseCombatWeapon *pWeapon );

	// UserCmd processing.
	virtual void	ProcessUsercmds( CUserCmd *cmds, int numcmds, int totalcmds, int dropped_packets, bool paused );

	// Frame updates.
	virtual void	PreThink();
	virtual void	PostThink();

	// Accessors.
	CHoloHand *		GetHandEntity() const				{ return m_hHand.Get(); }
	grid::CInventory &	GetInventory()					{ return _inventory; }

private:
	// UserCmd processing.
	holo::CFrame	AccumulateHoloFrame( CUserCmd *cmds, int numcmds, int totalcmds, int dropped_packets, bool paused );
	void			ProcessFrame( const holo::CFrame &frame );
	void			HandlePickupGesture();
	void			HandleGunGesture();

	CNetworkHandle( CHoloHand, m_hHand );				// The hand entity which is used to interact with the environment.

	grid::CGestureDetector	_gestureDetector;
	grid::CInventory	_inventory;

	bool			_weaponWasOut;
};

#endif // __GRID_PLAYER_H__