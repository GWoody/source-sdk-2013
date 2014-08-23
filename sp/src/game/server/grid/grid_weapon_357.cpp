/*
===============================================================================

	grid_weapon_357.cpp
	Defines the 357 weapon.

===============================================================================
*/

#include "cbase.h"
#include "grid_base_weapon.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridWeapon357 : public CGridBaseWeapon
{
public:
	DECLARE_CLASS( CGridWeapon357, CGridBaseWeapon );

	CGridWeapon357();
};

LINK_ENTITY_TO_CLASS( grid_weapon_357, CGridWeapon357 );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGridWeapon357::CGridWeapon357() : CGridBaseWeapon( "scripts\\grid_weapon_357.txt" )
{
}