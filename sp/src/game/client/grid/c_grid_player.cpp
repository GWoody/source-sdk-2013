/*
===============================================================================

	c_grid_player.cpp
	Client side implementation of the Grid player entity.

===============================================================================
*/

#include "cbase.h"
#include "c_grid_player.h"
#include "holodeck/out_etactor.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( player, C_GridPlayer );

// Network table.
IMPLEMENT_CLIENTCLASS_DT( C_GridPlayer, DT_GridPlayer, CGridPlayer )

	RecvPropArray3( RECVINFO_ARRAY( m_hHand ), RecvPropEHandle( RECVINFO(m_hHand[0]) ) ),
	RecvPropEHandle( RECVINFO( _activeWeapon ) ),
	RecvPropVector( RECVINFO( _viewoffset ) ),
	RecvPropDataTable( RECVINFO_DT(_haptics), 0, &REFERENCE_RECV_TABLE(DT_HoloHaptics) ),

END_RECV_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
C_GridPlayer::C_GridPlayer()
{
	_viewoffset.Init( 0, 0, 0 );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_GridPlayer::Simulate()
{
	_haptics.Update();
	BaseClass::Simulate();
}