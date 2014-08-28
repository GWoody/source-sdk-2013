/*
===============================================================================

	c_grid_weapo_357.cpp
	Client side implementation of the 357.

===============================================================================
*/

#include "cbase.h"
#include "c_grid_base_weapon.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridWeapon357 : public C_GridBaseWeapon
{
public:
	DECLARE_CLASS( C_GridWeapon357, C_GridBaseWeapon );
	DECLARE_CLIENTCLASS();
};

//-----------------------------------------------------------------------------
// Networking.
//-----------------------------------------------------------------------------
IMPLEMENT_CLIENTCLASS_DT( C_GridWeapon357, DT_GridWeapon357, CGridWeapon357 )

END_RECV_TABLE()

LINK_ENTITY_TO_CLASS( grid_weapon_357, C_GridWeapon357 );