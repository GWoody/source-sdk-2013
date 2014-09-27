/*
===============================================================================

	c_holo_hand.cpp
	Client side implementation of the Holodeck hand entity.

===============================================================================
*/

#include "cbase.h"
#include "c_holo_hand.h"
#include "out_etactor.h"

//-----------------------------------------------------------------------------
// Network table.
//-----------------------------------------------------------------------------
IMPLEMENT_CLIENTCLASS_DT( C_HoloHand, DT_HoloHand, CBaseHoloHand )

	RecvPropDataTable( RECVINFO_DT(_haptics), 0, &REFERENCE_RECV_TABLE(DT_HoloHaptics) ),

END_RECV_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_HoloHand::Simulate()
{
	_haptics.Update();
	BaseClass::Simulate();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_HoloHand::PostDataUpdate( DataUpdateType_t updateType )
{
	BaseClass::PostDataUpdate( updateType );
	_haptics.SetTargetId();
}