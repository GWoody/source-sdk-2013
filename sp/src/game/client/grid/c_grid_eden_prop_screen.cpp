/*
===============================================================================

	c_grid_eden_prop_screen.cpp
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
class C_GridEdenPropScreen : public CHoloVGuiScreenPanel
{
public:
	DECLARE_CLASS( C_GridEdenPropScreen, CHoloVGuiScreenPanel );
	
	C_GridEdenPropScreen( Panel *parent, const char *panelname );

	virtual void	OnTick();
};

// Expose the VGUI screen to the server.
DECLARE_VGUI_SCREEN_FACTORY( C_GridEdenPropScreen, "grid_eden_prop_screen" );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
C_GridEdenPropScreen::C_GridEdenPropScreen( Panel *parent, const char *panelname ) :
	BaseClass( parent, panelname )
{

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_GridEdenPropScreen::OnTick()
{
	SetBgColor( Color( 0, 0, 0, 63 ) );
	BaseClass::OnTick();
}