/*
===============================================================================

	c_holo_player.cpp
	Client side implementation of the Grid player entity.

===============================================================================
*/

#include "cbase.h"
#include "c_holo_player.h"
#include "out_etactor.h"

//-----------------------------------------------------------------------------
// Network table.
//-----------------------------------------------------------------------------
IMPLEMENT_CLIENTCLASS_DT( C_HoloPlayer, DT_HoloPlayer, CHoloPlayer )

	RecvPropArray3( RECVINFO_ARRAY( m_hHand ), RecvPropEHandle( RECVINFO(m_hHand[0]) ) ),
	RecvPropVector( RECVINFO( _viewoffset ) ),
	RecvPropDataTable( RECVINFO_DT(_haptics), 0, &REFERENCE_RECV_TABLE(DT_HoloHaptics) ),

END_RECV_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
C_HoloPlayer::C_HoloPlayer()
{
	_viewoffset.Init( 0, 0, 0 );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_HoloPlayer::Simulate()
{
	_haptics.Update();
	BaseClass::Simulate();
}