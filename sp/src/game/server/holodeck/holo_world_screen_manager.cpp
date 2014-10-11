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
CHoloWorldScreenManager::CHoloWorldScreenManager( CHoloPlayer *owner )
{
	_owner = owner;

	memset( _screens, 0, sizeof(_screens) );

	ListenForGameEvent( "holo_open_screen" );
	ListenForGameEvent( "holo_destroy_screen" );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloWorldScreenManager::FireGameEvent( IGameEvent *event )
{
	if( !Q_stricmp( event->GetName(), "holo_open_screen" ) )
	{
		EWorldPanel panel = (EWorldPanel)event->GetInt( "panel", WORLD_PANEL_MIDDLE );
		const char *type = event->GetString( "type", NULL );
		CreateScreen( panel, type );
	}
	else if( !Q_stricmp( event->GetName(), "holo_destroy_screen" ) )
	{
		int panel = (EWorldPanel)event->GetInt( "panel", -1 );
		if( panel != -1 )
		{
			DestroyScreen( (EWorldPanel)panel );
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CHoloWorldScreenManager::CreateScreen( EWorldPanel panel, const char *typeName )
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

	_screens[panel] = (CHoloWorldScreen *)CreateVGuiScreen( "holo_world_screen", typeName, _owner, _owner, -1 );

	if( _screens[panel] )
	{
		_screens[panel]->SetType( panel );
		_screens[panel]->SetActive( true );
		_screens[panel]->SetActualSize( 16, 12 );
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloWorldScreenManager::DestroyScreen( EWorldPanel panel )
{
	DestroyVGuiScreen( _screens[panel] );
	_screens[panel] = NULL;
}