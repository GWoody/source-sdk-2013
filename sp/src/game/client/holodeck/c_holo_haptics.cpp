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

	RecvPropInt( RECVINFO(_target) ),
	RecvPropInt( RECVINFO(_power) ),
	RecvPropInt( RECVINFO(_frequency) ),
	RecvPropBool( RECVINFO(_enabled) ),

END_RECV_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
C_HoloHaptics::C_HoloHaptics()
{
	_target = 0;
	_power = _frequency = 0;
	_enabled = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_HoloHaptics::Update()
{
	IETactor &tactor = IETactor::Get();
	tactor.SetState( _target, _enabled, _power, _frequency );
}