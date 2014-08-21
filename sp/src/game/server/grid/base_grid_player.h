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

	// GRID SPECIFIC METHODS.
	void			SetAttemptObjectPickup( bool pickup )	{ _attemptingPickup = pickup; }
	bool			IsAttemptingObjectPickup()				{ return _attemptingPickup; }

	// Object interaction.
	virtual void	PickupObject( CBaseEntity *pObject, bool bLimitMassAndSize );
	virtual bool	CanPickupObject( CBaseEntity *pObject, float massLimit, float sizeLimit );
	virtual void	PlayerUse();
	virtual CBaseEntity *	FindUseEntity();
	virtual bool	IsHoldingEntity( CBaseEntity *pEnt );
	virtual float	GetHeldObjectMass( IPhysicsObject *pHeldObject );

private:
	float			IntervalDistance( float x, float x0, float x1 );

	// GRID SPECIFIC VARIABLES.
	bool			_attemptingPickup, _lastAttemptingPickup;
};

#endif // __BASE_GRID_PLAYER_H__