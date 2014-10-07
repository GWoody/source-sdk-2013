/*
===============================================================================

	holo_world_screen_manager.h
	Handles the 3 VGUI panels that are children of a player.

===============================================================================
*/

#include "cbase.h"
#include "holo_player.h"
#include "holo_world_screen.h"
#include "holo_world_screen_manager.h"
#include "vguiscreen.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CHoloWorldScreenManager::CHoloWorldScreenManager()
{
	memset( _screens, 0, sizeof(_screens) );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CHoloWorldScreenManager::CreateScreen( holo::EWorldPanel panel, const char *typeName, CHoloPlayer *owner )
{
	if( !typeName )
	{
		ConColorMsg( COLOR_YELLOW, __FUNCTION__": called with a NULL typename!\n" );
		return false;
	}

	if( _screens[panel] )
	{
		DestroyScreen( panel );
	}

	_screens[panel] = (CHoloWorldScreen *)CreateVGuiScreen( "holo_world_screen", typeName, owner, owner, -1 );

	if( _screens[panel] )
	{
		_screens[panel]->SetType( panel );
		_screens[panel]->SetActive( true );
		_screens[panel]->SetActualSize( 32, 24 );
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloWorldScreenManager::DestroyScreen( holo::EWorldPanel panel )
{
	DestroyVGuiScreen( _screens[panel] );
	_screens[panel] = NULL;
}