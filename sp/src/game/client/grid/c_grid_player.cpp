/*
===============================================================================

	c_grid_player.cpp
	Client side implementation of the Grid player entity.

===============================================================================
*/

#include "cbase.h"
#include "c_grid_player.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( player, C_GridPlayer );

// Network table.
IMPLEMENT_CLIENTCLASS_DT( C_GridPlayer, DT_GridPlayer, CGridPlayer )

	RecvPropEHandle( RECVINFO( m_hHand ) ),
	RecvPropEHandle( RECVINFO( _activeWeapon ) ),
	RecvPropVector( RECVINFO( _viewoffset ) ),

END_RECV_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
C_GridPlayer::C_GridPlayer()
{
	_viewoffset.Init( 0, 0, 0 );
}