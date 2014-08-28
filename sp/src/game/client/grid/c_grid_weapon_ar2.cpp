/*
===============================================================================

	c_grid_weapo_ar2.cpp
	Client side implementation of the AR2.

===============================================================================
*/

#include "cbase.h"
#include "c_grid_base_weapon.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridWeaponAR2 : public C_GridBaseWeapon
{
public:
	DECLARE_CLASS( C_GridWeaponAR2, C_GridBaseWeapon );
	DECLARE_CLIENTCLASS();
};

//-----------------------------------------------------------------------------
// Networking.
//-----------------------------------------------------------------------------
IMPLEMENT_CLIENTCLASS_DT( C_GridWeaponAR2, DT_GridWeaponAr2, CGridWeaponAR2 )

END_RECV_TABLE()

LINK_ENTITY_TO_CLASS( grid_weapon_ar2, C_GridWeaponAR2 );