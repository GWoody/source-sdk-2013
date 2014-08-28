/*
===============================================================================

	c_grid_base_weapon.cpp
	Handles grid weapon effects.

===============================================================================
*/

#include "cbase.h"
#include "c_grid_base_weapon.h"
#include "c_te_legacytempents.h"

//-----------------------------------------------------------------------------
// Source Networking
//-----------------------------------------------------------------------------
IMPLEMENT_CLIENTCLASS_DT( C_GridBaseWeapon, DT_GridBaseWeapon, CGridBaseWeapon )

	RecvPropBool( RECVINFO( _triggerHeld ) ),
	RecvPropInt( RECVINFO( _remainingShots ) ),

END_RECV_TABLE()