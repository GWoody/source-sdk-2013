/*
===============================================================================

	grid_weapon_ar2.cpp
	Defines the AR2 weapon.

===============================================================================
*/

#include "cbase.h"
#include "grid_base_weapon.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridWeaponAR2 : public CGridBaseWeapon
{
public:
	DECLARE_CLASS( CGridWeaponAR2, CGridBaseWeapon );
	DECLARE_SERVERCLASS();

	CGridWeaponAR2();
};

IMPLEMENT_SERVERCLASS_ST( CGridWeaponAR2, DT_GridWeaponAR2 )
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( grid_weapon_ar2, CGridWeaponAR2 );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGridWeaponAR2::CGridWeaponAR2() : CGridBaseWeapon( "scripts\\grid_weapon_ar2.txt" )
{
}