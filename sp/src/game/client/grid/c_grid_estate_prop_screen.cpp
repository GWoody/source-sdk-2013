/*
===============================================================================

	c_grid_estate_prop_screen.cpp
	Implements the etactor calibration screen.

===============================================================================
*/

#include "cbase.h"
#include "c_vguiscreen.h"
#include "c_grid_player.h"
#include "holodeck/c_holo_world_screen_panel.h"

#include "filesystem.h"
#include "view_shared.h"
#include "view.h"
#include "model_types.h"
#include "materialsystem/itexture.h"
#include "rendertexture.h"
#include "vgui_bitmapbutton.h"
#include "tier1/fmtstr.h"

#include <vgui_controls/Panel.h>
#include <vgui_controls/Label.h>

using namespace vgui;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridEstatePropScreen : public CHoloVGuiScreenPanel
{
public:
	DECLARE_CLASS( C_GridEstatePropScreen, CHoloVGuiScreenPanel );
	
	C_GridEstatePropScreen( Panel *parent, const char *panelname );

	virtual void	OnTick();
	virtual void	OnCommand( const char *command );

private:
	void			OpenPanel( const char *type, EWorldPanel panel );
};

// Expose the VGUI screen to the server.
DECLARE_VGUI_SCREEN_FACTORY( C_GridEstatePropScreen, "grid_estate_prop_screen" );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridEstatePropSpawnScreen : public CHoloVGuiScreenPanel
{
public:
	DECLARE_CLASS( C_GridEstatePropSpawnScreen, CHoloVGuiScreenPanel );
	
	C_GridEstatePropSpawnScreen( Panel *parent, const char *panelname ) :
		BaseClass( parent, panelname )
	{
	}

	virtual void ApplySchemeSettings( IScheme *scheme )
	{
		BaseClass::ApplySchemeSettings( scheme );

		//
		// Open the script file.
		//
		KeyValues *kv = new KeyValues( "props" );
		if( !kv->LoadFromFile( g_pFullFileSystem, _script.Get(), "MOD" ) )
		{
			ConColorMsg( COLOR_YELLOW, __FUNCTION__": Unable to open \"%s\"!\n", _script.Get() );
			return;
		}

		//
		// Load all props.
		//
		int x = _propx;
		int y = _propy;
		FOR_EACH_TRUE_SUBKEY( kv, sub )
		{
			const char *modelname = sub->GetName();			
			CBitmapButton *button = new CBitmapButton( this, "model", "" );

			char modelnameNoExt[MAX_PATH];
			Q_StripExtension( modelname, modelnameNoExt, sizeof(modelnameNoExt) );
			CFmtStr path( "grid/%s.vmt", modelnameNoExt );

			// Set command.
			CUtlString command( "proptool " );
			command += modelname;
			button->SetCommand( command.Get() );

			// Set size.
			button->SetPos( x, y );
			button->SetSize( _buttonWidth, _buttonHeight );
			
			// Set images.
			color32 white = { 255, 255, 255, 255 };
			button->SetImage( CBitmapButton::BUTTON_DISABLED, path, white );
			button->SetImage( CBitmapButton::BUTTON_ENABLED, path, white );
			button->SetImage( CBitmapButton::BUTTON_ENABLED_MOUSE_OVER, path, white );
			button->SetImage( CBitmapButton::BUTTON_PRESSED, path, white );

			// Configure for next prop.
			x += _buttonWidth + _spacing;
			if( x + _buttonWidth >= GetWide() )
			{
				x = _propx;
				y += _buttonHeight + _spacing;
			}
		}

		kv->deleteThis();
	}

	virtual void ApplySettings( KeyValues *inResourceData )
	{
		BaseClass::ApplySettings( inResourceData );

		//
		// Get the prop script filename.
		//
		const char *propfilename = inResourceData->GetString( "propfile", NULL );	
		if( !propfilename )
		{
			ConColorMsg( COLOR_YELLOW, __FUNCTION__": Created prop spawn panel with no prop script set!\n" );
		}
		else
		{
			_script.Set( propfilename );
		}

		//
		// Get misc.
		//
		_spacing = inResourceData->GetInt( "spacing", 0 );
		_propx = inResourceData->GetInt( "propx", 10 );
		_propy = inResourceData->GetInt( "propy", 30 );
		_buttonWidth = inResourceData->GetInt( "buttonwidth", 64 );
		_buttonHeight = inResourceData->GetInt( "buttonheight", 64 );
	}

	virtual void OnTick()
	{
		SetBgColor( Color( 0, 0, 0, 63 ) );
		BaseClass::OnTick();
	}

	virtual void OnCommand( const char *command )
	{
		if( !Q_strncmp( "proptool ", command, 9 ) )
		{
			IGameEvent *event = gameeventmanager->CreateEvent( "grid_ready_proptool" );
			if( event )
			{
				event->SetString( "prop", command + 9 );
				gameeventmanager->FireEvent( event );
			}
			else
			{
				ConColorMsg( COLOR_YELLOW, __FUNCTION__": failed to create event!\n" );
			}

			event = gameeventmanager->CreateEvent( "holo_destroy_screen" );
			if( event )
			{
				event->SetInt( "panel", WORLD_PANEL_MIDDLE );
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

private:
	CUtlString		_script;
	int				_spacing;
	int				_buttonWidth;
	int				_buttonHeight;
	int				_propx;
	int				_propy;
};

typedef C_GridEstatePropSpawnScreen C_GridEstateSittingPropScreen;
typedef C_GridEstatePropSpawnScreen C_GridEstateSleepingPropScreen;
typedef C_GridEstatePropSpawnScreen C_GridEstateEntertainmentPropScreen;
typedef C_GridEstatePropSpawnScreen C_GridEstateSurfacePropScreen;
typedef C_GridEstatePropSpawnScreen C_GridEstateStoragePropScreen;

// Expose the VGUI screen to the server.
DECLARE_VGUI_SCREEN_FACTORY( C_GridEstateSittingPropScreen, "grid_estate_sitting_prop_screen" );
DECLARE_VGUI_SCREEN_FACTORY( C_GridEstateSleepingPropScreen, "grid_estate_sleeping_prop_screen" );
DECLARE_VGUI_SCREEN_FACTORY( C_GridEstateEntertainmentPropScreen, "grid_estate_entertainment_prop_screen" );
DECLARE_VGUI_SCREEN_FACTORY( C_GridEstateSurfacePropScreen, "grid_estate_surface_prop_screen" );
DECLARE_VGUI_SCREEN_FACTORY( C_GridEstateStoragePropScreen, "grid_estate_storage_prop_screen" );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
C_GridEstatePropScreen::C_GridEstatePropScreen( Panel *parent, const char *panelname ) :
	BaseClass( parent, panelname )
{

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_GridEstatePropScreen::OnTick()
{
	SetBgColor( Color( 0, 0, 0, 63 ) );
	BaseClass::OnTick();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_GridEstatePropScreen::OnCommand( const char *command )
{
	if( !Q_stricmp( command, "open_sitting_props" ) )
	{
		OpenPanel( "grid_estate_sitting_prop_screen", WORLD_PANEL_MIDDLE );
	}
	else if( !Q_stricmp( command, "open_sleeping_props" ) )
	{
		OpenPanel( "grid_estate_sleeping_prop_screen", WORLD_PANEL_MIDDLE );
	}
	else if( !Q_stricmp( command, "open_entertainment_props" ) )
	{
		OpenPanel( "grid_estate_entertainment_prop_screen", WORLD_PANEL_MIDDLE );
	}
	else if( !Q_stricmp( command, "open_surface_props" ) )
	{
		OpenPanel( "grid_estate_surface_prop_screen", WORLD_PANEL_MIDDLE );
	}
	else if( !Q_stricmp( command, "open_storage_props" ) )
	{
		OpenPanel( "grid_estate_storage_prop_screen", WORLD_PANEL_MIDDLE );
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_GridEstatePropScreen::OpenPanel( const char *type, EWorldPanel panel )
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