/*
===============================================================================

	c_grid_base_weapon.h
	Handles grid weapon effects.

===============================================================================
*/

#ifndef __C_GRID_BASE_WEAPON_H__
#define __C_GRID_BASE_WEAPON_H__

#include "c_baseanimating.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridBaseWeapon : public C_BaseAnimating
{
public:
	DECLARE_CLASS( C_GridBaseWeapon, C_BaseAnimating );
	DECLARE_CLIENTCLASS();
	
	// Accessors.
	bool			IsTriggerHeld() const				{ return _triggerHeld; }
	int				GetRemainingShots() const			{ return _remainingShots; }

private:
	bool			_triggerHeld;
	int				_remainingShots;
};

#endif // __C_GRID_BASE_WEAPON_H__