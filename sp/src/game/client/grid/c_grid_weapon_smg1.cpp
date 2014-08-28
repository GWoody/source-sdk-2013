/*
===============================================================================

	c_grid_weapon_smg1.cpp
	Client side implementation of the SMG1.

===============================================================================
*/

#include "cbase.h"
#include "c_grid_base_weapon.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridWeaponSMG1 : public C_GridBaseWeapon
{
public:
	DECLARE_CLASS( C_GridWeaponSMG1, C_GridBaseWeapon );
	DECLARE_CLIENTCLASS();
};

//-----------------------------------------------------------------------------
// Networking.
//-----------------------------------------------------------------------------
IMPLEMENT_CLIENTCLASS_DT( C_GridWeaponSMG1, DT_GridWeaponSMG1, CGridWeaponSMG1 )

END_RECV_TABLE()

LINK_ENTITY_TO_CLASS( grid_weapon_smg1, C_GridWeaponSMG1 );