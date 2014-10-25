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

	RecvPropEHandle( RECVINFO( _activeWeapon ) ),
	RecvPropEHandle( RECVINFO( _prop ) ),

END_RECV_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
C_GridPlayer::C_GridPlayer()
{
}