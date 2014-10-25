/*
===============================================================================

	c_grid_estate_prop_screen.cpp
	Implements the etactor calibration screen.

===============================================================================
*/

#include "cbase.h"
#include "c_vguiscreen.h"
#include "holodeck/c_holo_world_screen_panel.h"

#include <vgui_controls/Slider.h>
using namespace vgui;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridEstatePropPositionScreen : public CHoloVGuiScreenPanel
{
public:
	DECLARE_CLASS( C_GridEstatePropPositionScreen, CHoloVGuiScreenPanel );
	
	C_GridEstatePropPositionScreen( Panel *parent, const char *panelname ) :
		BaseClass( parent, panelname )
	{

	}

	virtual void OnTick()
	{
		// Commit prop settings.
		IGameEvent *event = gameeventmanager->CreateEvent( "grid_prop_offset" );
		if( event )
		{
			event->SetFloat( "x", _x->GetValue() );
			event->SetFloat( "y", _y->GetValue() );
			event->SetFloat( "z", _z->GetValue() );
			gameeventmanager->FireEvent( event );
		}
		else
		{
			ConColorMsg( COLOR_YELLOW, __FUNCTION__": failed to create event!\n" );
		}

		SetBgColor( Color( 0, 0, 0, 63 ) );
		BaseClass::OnTick();
	}

	virtual void OnCommand( const char *command )
	{
		if( !Q_stricmp( command, "close" ) )
		{
			// Commit prop settings.
			IGameEvent *event = gameeventmanager->CreateEvent( "grid_commit_prop" );
			if( event )
			{
				gameeventmanager->FireEvent( event );
			}
			else
			{
				ConColorMsg( COLOR_YELLOW, __FUNCTION__": failed to create event!\n" );
			}

			// Chain to base the handle the actual close.
			BaseClass::OnCommand( command );
		}
		else
		{
			BaseClass::OnCommand( command );
		}
	}

	virtual void ApplySchemeSettings( IScheme *scheme )
	{
		BaseClass::ApplySchemeSettings( scheme );

		_x = dynamic_cast<Slider *>( FindChildByName( "XSlider" ) );
		_y = dynamic_cast<Slider *>( FindChildByName( "YSlider" ) );
		_z = dynamic_cast<Slider *>( FindChildByName( "ZSlider" ) );
		Assert( _x && _y && _z );
	}

private:
	Slider *		_x;
	Slider *		_y;
	Slider *		_z;
};

// Expose the VGUI screen to the server.
DECLARE_VGUI_SCREEN_FACTORY( C_GridEstatePropPositionScreen, "grid_estate_prop_position_screen" );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridEstatePropAngleScreen : public CHoloVGuiScreenPanel
{
public:
	DECLARE_CLASS( C_GridEstatePropAngleScreen, CHoloVGuiScreenPanel );
	
	C_GridEstatePropAngleScreen( Panel *parent, const char *panelname ) :
		BaseClass( parent, panelname )
	{

	}

	virtual void OnTick()
	{
		// Commit prop settings.
		IGameEvent *event = gameeventmanager->CreateEvent( "grid_prop_angles" );
		if( event )
		{
			event->SetFloat( "pitch", _pitch->GetValue() );
			event->SetFloat( "roll", _roll->GetValue() );
			event->SetFloat( "yaw", _yaw->GetValue() );
			gameeventmanager->FireEvent( event );
		}
		else
		{
			ConColorMsg( COLOR_YELLOW, __FUNCTION__": failed to create event!\n" );
		}

		SetBgColor( Color( 0, 0, 0, 63 ) );
		BaseClass::OnTick();
	}

	virtual void OnCommand( const char *command )
	{
		if( !Q_stricmp( command, "close" ) )
		{
			// Commit prop settings.
			IGameEvent *event = gameeventmanager->CreateEvent( "grid_commit_prop" );
			if( event )
			{
				gameeventmanager->FireEvent( event );
			}
			else
			{
				ConColorMsg( COLOR_YELLOW, __FUNCTION__": failed to create event!\n" );
			}

			// Chain to base the handle the actual close.
			BaseClass::OnCommand( command );
		}
		else
		{
			BaseClass::OnCommand( command );
		}
	}

	virtual void ApplySchemeSettings( IScheme *scheme )
	{
		BaseClass::ApplySchemeSettings( scheme );

		_pitch = dynamic_cast<Slider *>( FindChildByName( "PitchSlider" ) );
		_roll = dynamic_cast<Slider *>( FindChildByName( "RollSlider" ) );
		_yaw = dynamic_cast<Slider *>( FindChildByName( "YawSlider" ) );
		Assert( _pitch && _roll && _yaw );
	}

private:
	Slider *		_pitch;
	Slider *		_roll;
	Slider *		_yaw;
};

// Expose the VGUI screen to the server.
DECLARE_VGUI_SCREEN_FACTORY( C_GridEstatePropAngleScreen, "grid_estate_prop_angle_screen" );