/*
===============================================================================

	c_holo_haptics.cpp
	Implements the client side Holodeck haptic class.

===============================================================================
*/

#include "cbase.h"
#include "c_holo_haptics.h"
#include "out_etactor.h"
#include "holodeck/holo_shared.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static ConVar holo_etactor_left_hand_id( "holo_etactor_left_hand_id", "2", FCVAR_ARCHIVE );
static ConVar holo_etactor_right_hand_id( "holo_etactor_right_hand_id", "1", FCVAR_ARCHIVE );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
BEGIN_RECV_TABLE_NOBASE( C_HoloHaptics, DT_HoloHaptics )

	RecvPropInt( RECVINFO(_targetHand) ),
	RecvPropInt( RECVINFO(_power) ),
	RecvPropInt( RECVINFO(_frequency) ),
	RecvPropBool( RECVINFO(_enabled) ),

END_RECV_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
C_HoloHaptics::C_HoloHaptics()
{
	_targetHand = HAND_LEFT;
	_targetId = -1;
	_power = _frequency = 0;
	_enabled = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_HoloHaptics::Update()
{
	if( _targetId < 0 )
	{
		return;
	}

	IETactor &tactor = IETactor::Get();
	tactor.SetState( _targetId, _enabled, _power, _frequency );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_HoloHaptics::SetTargetId()
{
	switch( _targetHand )
	{
		case HAND_LEFT:
			_targetId = holo_etactor_left_hand_id.GetInt();
			break;

		case HAND_RIGHT:
			_targetId = holo_etactor_right_hand_id.GetInt();
			break;
	}
}