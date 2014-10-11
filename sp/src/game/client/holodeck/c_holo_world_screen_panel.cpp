/*
===============================================================================

	c_holo_world_screen_panel.cpp
	Implements the base class for a panel in world space that is a child of the player.

===============================================================================
*/

#include "cbase.h"
#include "c_holo_world_screen_panel.h"
#include "c_holo_world_screen.h"

#include <vgui_controls/Panel.h>
#include <vgui/IVGui.h>
using namespace vgui;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CHoloVGuiScreenPanel::CHoloVGuiScreenPanel( Panel *parent, const char *panelname ) :
	BaseClass( parent, panelname )
{

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CHoloVGuiScreenPanel::Init( KeyValues *kv, VGuiScreenInitData_t *init )
{
	ivgui()->AddTickSignal( GetVPanel() );
	SetVisible( true );

	return BaseClass::Init( kv, init );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloVGuiScreenPanel::OnCommand( const char *command )
{
	if( !Q_stricmp( command, "close" ) )
	{
		ivgui()->RemoveTickSignal( GetVPanel() );
		
		IGameEvent *event = gameeventmanager->CreateEvent( "holo_destroy_screen" );
		if( event )
		{
			C_HoloWorldScreen *screen = (C_HoloWorldScreen *)GetEntity();
			event->SetInt( "panel", screen->GetWorldPanelType() );
			gameeventmanager->FireEvent( event );
		}
		else
		{
			ConColorMsg( COLOR_YELLOW, __FUNCTION__": failed to create event!\n" );
		}		
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}