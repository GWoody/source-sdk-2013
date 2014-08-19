/*
===============================================================================

	grid_player.h
	Implements functionality specific for the Grid player.

===============================================================================
*/

#ifndef __GRID_PLAYER_H__
#define __GRID_PLAYER_H__

#include "player.h"
#include "holodeck/holo_hand.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridPlayer : public CBasePlayer
{
public:
	DECLARE_CLASS( CGridPlayer, CBasePlayer );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

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

	// Object interaction.
	virtual void	PickupObject( CBaseEntity *pObject, bool bLimitMassAndSize );
	virtual bool	CanPickupObject( CBaseEntity *pObject, float massLimit, float sizeLimit );
	virtual void	PlayerUse();
	virtual CBaseEntity *	FindUseEntity();
	virtual bool	IsHoldingEntity( CBaseEntity *pEnt );
	virtual float	GetHeldObjectMass( IPhysicsObject *pHeldObject );

	// Frame updates.
	virtual void	PreThink();
	virtual void	PostThink();

	// Accessors.
	CHoloHand *		GetHandEntity() const				{ return m_hHand.Get(); }

private:
	float			IntervalDistance( float x, float x0, float x1 );

private:
	CNetworkHandle( CHoloHand, m_hHand );				// The hand entity which is used to interact with the environment.
};

#endif // __GRID_PLAYER_H__