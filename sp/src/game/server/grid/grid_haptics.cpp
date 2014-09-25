/*
===============================================================================

	grid_haptics.cpp
	Implements the interface for the Grid haptic class.

===============================================================================
*/

#include "cbase.h"
#include "grid_haptics.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
BEGIN_SEND_TABLE_NOBASE( CGridHaptics, DT_GridHaptics )
	SendPropInt( SENDINFO(_power) ),
	SendPropInt( SENDINFO(_frequency) ),
	SendPropBool( SENDINFO(_enabled) ),
END_SEND_TABLE()

BEGIN_SIMPLE_DATADESC( CGridHaptics )
END_DATADESC()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGridHaptics::CGridHaptics()
{
	_power = _frequency = 0;
	_enabled = false;
}