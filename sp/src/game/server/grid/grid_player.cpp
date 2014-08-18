/*
===============================================================================

	grid_player.cpp
	Implements functionality specific for the Grid player.

===============================================================================
*/

#include "cbase.h"
#include "grid_player.h"

LINK_ENTITY_TO_CLASS( player, CGridPlayer );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridPlayer::Spawn()
{
	SetModel( "models/player.mdl" );
	BaseClass::Spawn();
}