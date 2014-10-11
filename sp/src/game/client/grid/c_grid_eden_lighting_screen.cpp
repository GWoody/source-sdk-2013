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

using namespace vgui;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridEdenLightingScreen : public CHoloVGuiScreenPanel
{
public:
	DECLARE_CLASS( C_GridEdenLightingScreen, CHoloVGuiScreenPanel );
	
	C_GridEdenLightingScreen( Panel *parent, const char *panelname );

	virtual void	OnTick();
};

// Expose the VGUI screen to the server.
DECLARE_VGUI_SCREEN_FACTORY( C_GridEdenLightingScreen, "grid_eden_lighting_screen" );

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
	SetBgColor( Color( 0, 0, 0, 63 ) );
	BaseClass::OnTick();
}