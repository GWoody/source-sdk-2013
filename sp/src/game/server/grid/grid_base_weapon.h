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
#include "vguiscreen.h"

class CGridPlayer;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridBaseWeapon : public CBaseAnimating
{
public:
	DECLARE_CLASS( CGridBaseWeapon, CBaseAnimating );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

					CGridBaseWeapon( const char *script );
	virtual			~CGridBaseWeapon();

	// Source overrides.
	virtual void	Precache();
	virtual void	Spawn();
	virtual void	Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );
	virtual int		ObjectCaps();

	// Player interaction.
	virtual void	SwapWith( CGridBaseWeapon *weapon );
	virtual void	Drop( const Vector &target );
	virtual void	Pickup( CGridPlayer *player );
	virtual void	TakeOut();
	virtual void	PutAway();

	// Accessors.
	int				GetBulletCount() const					{ return _remainingShots; }

	// Shooting.
	virtual void	SetTriggerState( bool pressed );

	// Weapon updates.
	void			SetDirection( const Vector &dir );
	virtual void	ItemPreFrame();

protected:
	// Sounds.
	void			PlayShootSound();
	void			PlayEmptySound();

	// Shooting.
	virtual void	ShootSingleBullet();
	virtual void	DoMuzzleFlash();
	virtual void	PerformImpactTrace();
	virtual void	EjectShell();

private:
	void			Shoot();

	// Screen.
	void			CreateAmmoScreen();

	// Weapon updates.
	void			CommitAngle();

	// Weapon class information.
	grid::CWeaponInfo	_info;

	// State information.
	CNetworkVar( bool, _triggerHeld );
	CNetworkVar( int, _remainingShots );
	float			_nextFireTime;
	Vector			_direction;
	bool			_firedSinceTrigger;		// Have we fired a trigger since the last time the trigger was pulled?

	CHandle<CVGuiScreen>	_ammoScreen;
};

#endif // __GRID_BASE_WEAPON_H__