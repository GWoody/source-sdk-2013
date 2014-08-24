/*
===============================================================================

	grid_weapon_smg1.cpp
	Defines the SMG1 weapon.

===============================================================================
*/

#include "cbase.h"
#include "grid_base_weapon.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridWeaponSMG1 : public CGridBaseWeapon
{
public:
	DECLARE_CLASS( CGridWeaponSMG1, CGridBaseWeapon );
	DECLARE_SERVERCLASS();

	CGridWeaponSMG1();
};

IMPLEMENT_SERVERCLASS_ST( CGridWeaponSMG1, DT_GridWeaponSMG1 )
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( grid_weapon_smg1, CGridWeaponSMG1 );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGridWeaponSMG1::CGridWeaponSMG1() : CGridBaseWeapon( "scripts\\grid_weapon_smg1.txt" )
{
}