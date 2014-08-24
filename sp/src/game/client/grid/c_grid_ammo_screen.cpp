/*
===============================================================================

	c_grid_ammo_screen.cpp
	Implements the weapon ammo counter.

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
class C_GridAmmoScreen : public CVGuiScreenPanel
{
public:
	DECLARE_CLASS( C_GridAmmoScreen, CVGuiScreenPanel );
	
	C_GridAmmoScreen( Panel *parent, const char *panelname );
	
	virtual bool	Init( KeyValues *kv, VGuiScreenInitData_t *init );
	virtual void	OnTick();
	
private:
	Label *			_ammoCount;
};

// Expose the VGUI screen to the server.
DECLARE_VGUI_SCREEN_FACTORY( C_GridAmmoScreen, "grid_ammo_screen" );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
C_GridAmmoScreen::C_GridAmmoScreen( Panel *parent, const char *panelname ) :
	BaseClass( parent, panelname )
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool C_GridAmmoScreen::Init( KeyValues *kv, VGuiScreenInitData_t *init )
{
	if( !BaseClass::Init( kv, init ) )
	{
		return false;
	}
	
	ivgui()->AddTickSignal( GetVPanel() );
	
	_ammoCount = dynamic_cast<Label *>( FindChildByName( "AmmoCount" ) );
	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_GridAmmoScreen::OnTick()
{
	SetBgColor( Color( 0, 0, 0, 128 ) );

	BaseClass::OnTick();

	C_GridPlayer *player = dynamic_cast<C_GridPlayer *>( C_BasePlayer::GetLocalPlayer() );
	Assert( player );

	C_GridBaseWeapon *weapon = player->GetActiveWeapon();
	if( !weapon )
	{
		SetVisible( false );
		return;
	}

	SetVisible( true );

	int ammocount = weapon->GetRemainingShots();
	if( _ammoCount )
	{
		char buf[32];
		Q_snprintf( buf, sizeof( buf ), "%d", ammocount );
		_ammoCount->SetText( buf );
	}
}