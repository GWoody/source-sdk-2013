/*
===============================================================================

	base_grid_player.h
    Defines player functionality borrowed and modified from HL2.

===============================================================================
*/

#ifndef __BASE_GRID_PLAYER_H__
#define __BASE_GRID_PLAYER_H__

#include "player.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CBaseGridPlayer : public CBasePlayer
{
public:
	DECLARE_CLASS( CBaseGridPlayer, CBasePlayer );

	CBaseGridPlayer();

	// Object interaction.
	virtual void	PlayerUse();
	virtual bool	IsHoldingEntity( CBaseEntity *pEnt );
	virtual float	GetHeldObjectMass( IPhysicsObject *pHeldObject );
};

#endif // __BASE_GRID_PLAYER_H__