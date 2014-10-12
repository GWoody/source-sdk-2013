/*
===============================================================================

	c_grid_eden_lighting_screen.cpp
	Implements the etactor calibration screen.

===============================================================================
*/

#include "cbase.h"
#include "c_vguiscreen.h"
#include "c_grid_player.h"
#include "holodeck/c_holo_world_screen_panel.h"

#include <vgui_controls/Panel.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>

using namespace vgui;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridEdenLightingScreen : public CHoloVGuiScreenPanel
{
public:
	DECLARE_CLASS( C_GridEdenLightingScreen, CHoloVGuiScreenPanel );
	
	C_GridEdenLightingScreen( Panel *parent, const char *panelname );

	virtual void	OnTick();
	virtual void	OnCommand( const char *command );
	virtual void	ApplySchemeSettings( IScheme *scheme );

protected:
	virtual const char *	GetLightingName() const = 0;

private:
	void			SetButtonColor( Button *btn, const Color &color );

	Label *			_header;
	Button *		_ambientButton;
	Button *		_brightnessButton;

	bool			_isAmbient;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridEdenSunLightingScreen : public C_GridEdenLightingScreen
{
public:
	DECLARE_CLASS( C_GridEdenSunLightingScreen, C_GridEdenLightingScreen );
	
	C_GridEdenSunLightingScreen( Panel *parent, const char *panelname ) :
		BaseClass( parent, panelname )
	{
	}

	virtual const char *GetLightingName() const
	{
		return "Sun Colour";
	}
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridEdenMoonLightingScreen : public C_GridEdenLightingScreen
{
public:
	DECLARE_CLASS( C_GridEdenMoonLightingScreen, C_GridEdenLightingScreen );
	
	C_GridEdenMoonLightingScreen( Panel *parent, const char *panelname ) :
		BaseClass( parent, panelname )
	{
	}

	virtual const char *GetLightingName() const
	{
		return "Moon Colour";
	}
};

// Expose the VGUI screen to the server.
DECLARE_VGUI_SCREEN_FACTORY( C_GridEdenSunLightingScreen, "grid_eden_sun_lighting_screen" );
DECLARE_VGUI_SCREEN_FACTORY( C_GridEdenMoonLightingScreen, "grid_eden_moon_lighting_screen" );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
C_GridEdenLightingScreen::C_GridEdenLightingScreen( Panel *parent, const char *panelname ) :
	BaseClass( parent, panelname )
{
	_isAmbient = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_GridEdenLightingScreen::OnTick()
{
	//
	// Highlight the mode the user is in.
	//
	if( _isAmbient && _ambientButton )
	{
		_ambientButton->ForceDepressed( true );
	}
	else if( _brightnessButton )
	{
		_brightnessButton->ForceDepressed( true );
	}

	Button *btn = dynamic_cast<Button *>( FindChildByName( "RedButton" ) );
	SetButtonColor( btn, Color(255, 0, 0, 255) );

	SetBgColor( Color( 0, 0, 0, 63 ) );
	BaseClass::OnTick();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_GridEdenLightingScreen::ApplySchemeSettings( IScheme *scheme )
{
	BaseClass::ApplySchemeSettings( scheme );

	_header = dynamic_cast<Label *>( FindChildByName( "HeaderLabel" ) );
	Assert( _header );
	_header->SetText( GetLightingName() );

	_ambientButton = dynamic_cast<Button *>( FindChildByName( "AmbientButton" ) );
	_brightnessButton = dynamic_cast<Button *>( FindChildByName( "BrightnessButton" ) );
	Assert( _ambientButton && _brightnessButton );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_GridEdenLightingScreen::OnCommand( const char *command )
{
	if( !Q_stricmp( command, "ambient" ) )
	{
		_isAmbient = true;
	}
	else if( !Q_stricmp( command, "brightness" ) )
	{
		_isAmbient = false;
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_GridEdenLightingScreen::SetButtonColor( Button *btn, const Color &color )
{
	btn->SetBgColor( color );
	btn->SetFgColor( color );
}