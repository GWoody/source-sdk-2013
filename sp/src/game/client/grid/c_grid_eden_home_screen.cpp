/*
===============================================================================

	c_grid_eden_home_screen.cpp
	Implements the etactor calibration screen.

===============================================================================
*/

#include "cbase.h"
#include "c_vguiscreen.h"
#include "c_grid_player.h"
#include "holodeck/c_holo_world_screen_panel.h"

#include <vgui_controls/Panel.h>
#include <vgui_controls/Label.h>

using namespace vgui;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridEdenHomeScreen : public CHoloVGuiScreenPanel
{
public:
	DECLARE_CLASS( C_GridEdenHomeScreen, CHoloVGuiScreenPanel );
	
	C_GridEdenHomeScreen( Panel *parent, const char *panelname );

	virtual void	OnTick();
	virtual void	OnCommand( const char *command );

private:
	void			OpenPanel( const char *type, EWorldPanel panel );
};

// Expose the VGUI screen to the server.
DECLARE_VGUI_SCREEN_FACTORY( C_GridEdenHomeScreen, "grid_eden_home_screen" );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
C_GridEdenHomeScreen::C_GridEdenHomeScreen( Panel *parent, const char *panelname ) :
	BaseClass( parent, panelname )
{

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_GridEdenHomeScreen::OnTick()
{
	SetBgColor( Color( 0, 0, 0, 63 ) );
	BaseClass::OnTick();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_GridEdenHomeScreen::OnCommand( const char *command )
{
	if( !Q_stricmp( command, "open_sun_color" ) )
	{
		OpenPanel( "grid_eden_sun_lighting_screen", WORLD_PANEL_RIGHT );
	}
	else if( !Q_stricmp( command, "open_moon_color" ) )
	{
		OpenPanel( "grid_eden_moon_lighting_screen", WORLD_PANEL_RIGHT );
	}
	else if( !Q_stricmp( command, "open_prop_spawner" ) )
	{
		OpenPanel( "grid_eden_prop_screen", WORLD_PANEL_LEFT );
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_GridEdenHomeScreen::OpenPanel( const char *type, EWorldPanel panel )
{
	IGameEvent *event = gameeventmanager->CreateEvent( "holo_open_screen" );
	if( !event )
	{
		ConColorMsg( COLOR_YELLOW, __FUNCTION__": failed to create event!\n" );
		return;
	}

	event->SetString( "type", type );
	event->SetInt( "panel", panel );

	gameeventmanager->FireEvent( event );
}