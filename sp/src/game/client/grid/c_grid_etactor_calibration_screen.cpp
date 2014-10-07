/*
===============================================================================

	c_grid_etactor_calibration_screen.cpp
	Implements the etactor calibration screen.

===============================================================================
*/

#include "cbase.h"
#include "c_vguiscreen.h"
#include "c_grid_player.h"

#include <vgui_controls/Panel.h>
#include <vgui_controls/Label.h>
#include <vgui/IVGui.h>

using namespace vgui;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridETactorScreen : public CVGuiScreenPanel
{
public:
	DECLARE_CLASS( C_GridETactorScreen, CVGuiScreenPanel );
	
	C_GridETactorScreen( Panel *parent, const char *panelname );

	virtual bool	Init( KeyValues *kv, VGuiScreenInitData_t *init );
	virtual void	OnTick();
};

// Expose the VGUI screen to the server.
DECLARE_VGUI_SCREEN_FACTORY( C_GridETactorScreen, "grid_etactor_calibration_screen" );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
C_GridETactorScreen::C_GridETactorScreen( Panel *parent, const char *panelname ) :
	BaseClass( parent, panelname )
{

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool C_GridETactorScreen::Init( KeyValues *kv, VGuiScreenInitData_t *init )
{
	if( !BaseClass::Init( kv, init ) )
	{
		return false;
	}
	
	ivgui()->AddTickSignal( GetVPanel() );
	SetVisible( true );

	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_GridETactorScreen::OnTick()
{
	SetBgColor( Color( 0, 0, 0, 63 ) );
	BaseClass::OnTick();
}