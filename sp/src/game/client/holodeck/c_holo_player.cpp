/*
===============================================================================

	c_holo_player.cpp
	Client side implementation of the Holodeck player entity.

===============================================================================
*/

#include "cbase.h"
#include "c_holo_player.h"
#include "out_etactor.h"

//-----------------------------------------------------------------------------
// Network table.
//-----------------------------------------------------------------------------
IMPLEMENT_CLIENTCLASS_DT( C_HoloPlayer, DT_HoloPlayer, CHoloPlayer )

	RecvPropArray3( RECVINFO_ARRAY( _hands ), RecvPropEHandle( RECVINFO(_hands[0]) ) ),
	RecvPropVector( RECVINFO( _viewoffset ) ),

END_RECV_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
C_HoloPlayer::C_HoloPlayer()
{
	_viewoffset.Init( 0, 0, 0 );
}