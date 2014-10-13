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
#include <vgui_controls/Slider.h>

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
	virtual bool	IsSun() const = 0;

private:
	Label *			_header;

	Slider *		_redSlider;
	Slider *		_greenSlider;
	Slider *		_blueSlider;
	Slider *		_alphaSlider;
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

	virtual bool IsSun() const
	{
		return true;
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

	virtual bool IsSun() const
	{
		return false;
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
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_GridEdenLightingScreen::OnTick()
{
	IGameEvent *event = gameeventmanager->CreateEvent( "grid_env_light_color" );
	if( event )
	{
		event->SetInt( "issun", IsSun() );
		event->SetInt( "isambient", false );
		event->SetFloat( "r", _redSlider->GetValue() );
		event->SetFloat( "g", _greenSlider->GetValue() );
		event->SetFloat( "b", _blueSlider->GetValue() );
		event->SetFloat( "a", _alphaSlider->GetValue() / 200.0f );

		gameeventmanager->FireEvent( event );
	}

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

	_redSlider = dynamic_cast<Slider *>( FindChildByName( "RedSlider" ) );
	_greenSlider = dynamic_cast<Slider *>( FindChildByName( "GreenSlider" ) );
	_blueSlider = dynamic_cast<Slider *>( FindChildByName( "BlueSlider" ) );
	_alphaSlider = dynamic_cast<Slider *>( FindChildByName( "AlphaSlider" ) );
	Assert( _redSlider && _greenSlider && _blueSlider );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_GridEdenLightingScreen::OnCommand( const char *command )
{
	BaseClass::OnCommand( command );
}