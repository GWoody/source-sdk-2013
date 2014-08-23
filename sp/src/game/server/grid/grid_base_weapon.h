/*
===============================================================================

	grid_base_weapon.h
	Implements functionality core to all grid weapons.

===============================================================================
*/

#ifndef __GRID_BASE_WEAPON_H__
#define __GRID_BASE_WEAPON_H__

#include "baseanimating.h"
#include "grid_weapon_info.h"

class CGridPlayer;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridBaseWeapon : public CBaseAnimating
{
public:
	DECLARE_CLASS( CGridBaseWeapon, CBaseAnimating );

					CGridBaseWeapon( const char *script );
	virtual			~CGridBaseWeapon();

	// Source overrides.
	virtual void	Precache();
	virtual void	Spawn();
	virtual void	Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );
	virtual int		ObjectCaps();

	// Player interaction.
	virtual void	Drop( const Vector &pos );
	virtual void	Pickup( CGridPlayer *player );
	virtual void	TakeOut();
	virtual void	PutAway();

	// Shooting.
	virtual void	SetTriggerState( bool pressed );

	// Weapon updates.
	void			SetDirection( const Vector &dir );
	virtual void	ItemPreFrame();

protected:
	// Sounds.
	void			PlayShootSound();
	void			PlayEmptySound();

	virtual void	ShootSingleBullet();

	virtual void	DoMuzzleFlash();
	virtual void	MakeTracer( const Vector &start, const Vector &end );

private:
	void			Shoot();

	// Weapon updates.
	void			CommitAngle();

	// Weapon class information.
	grid::CWeaponInfo	_info;

	// State information.
	bool			_triggerHeld;
	int				_remainingShots;
	float			_nextFireTime;
	Vector			_direction;
};

#endif // __GRID_BASE_WEAPON_H__