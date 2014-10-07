/*
===============================================================================

	holo_world_screen.cpp
	Implements the base class for a panel in world space that is a child of the player.

===============================================================================
*/

#include "cbase.h"
#include "holodeck/holo_world_screen.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IMPLEMENT_SERVERCLASS_ST( CHoloWorldScreen, DT_HoloWorldScreen )

	SendPropInt( SENDINFO( _type ) ),

END_SEND_TABLE();

BEGIN_DATADESC( CHoloWorldScreen )
END_DATADESC()

LINK_ENTITY_TO_CLASS( holo_world_screen, CHoloWorldScreen );
LINK_ENTITY_TO_CLASS( vgui_screen, CHoloWorldScreen );
LINK_ENTITY_TO_CLASS( vgui_screen_team, CHoloWorldScreen );
PRECACHE_REGISTER( vgui_screen );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CHoloWorldScreen::CHoloWorldScreen()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloWorldScreen::Spawn()
{
	BaseClass::Spawn();
}