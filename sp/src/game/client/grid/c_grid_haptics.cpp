/*
===============================================================================

	c_grid_haptics.cpp
	Implements the client side Grid haptic class.

===============================================================================
*/

#include "cbase.h"
#include "c_grid_haptics.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
BEGIN_RECV_TABLE_NOBASE( C_GridHaptics, DT_GridHaptics )
	RecvPropInt( RECVINFO(_power) ),
	RecvPropInt( RECVINFO(_frequency) ),
	RecvPropBool( RECVINFO(_enabled) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
C_GridHaptics::C_GridHaptics()
{
	_power = _frequency = 0;
	_enabled = false;
}