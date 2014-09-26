/*
===============================================================================

	c_holo_haptics.cpp
	Implements the client side Holodeck haptic class.

===============================================================================
*/

#include "cbase.h"
#include "c_holo_haptics.h"
#include "holodeck/out_etactor.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
BEGIN_RECV_TABLE_NOBASE( C_HoloHaptics, DT_HoloHaptics )

	RecvPropInt( RECVINFO(_power) ),
	RecvPropInt( RECVINFO(_frequency) ),
	RecvPropBool( RECVINFO(_enabled) ),

END_RECV_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
C_HoloHaptics::C_HoloHaptics()
{
	_power = _frequency = 0;
	_enabled = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_HoloHaptics::Update()
{
	IETactor &tactor = IETactor::Get();
	tactor.SetPower( _power );
	tactor.SetFrequency( _frequency );
	tactor.SetEnabled( _enabled );
}