/*
===============================================================================

	c_holo_world_screen_panel.h
	Implements the base class for a panel in world space that is a child of the player.

===============================================================================
*/

#ifndef __C_HOLO_WORLD_SCREEN_PANEL_H__
#define __C_HOLO_WORLD_SCREEN_PANEL_H__

#include "c_vguiscreen.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CHoloVGuiScreenPanel : public CVGuiScreenPanel
{
public:
	DECLARE_CLASS( CHoloVGuiScreenPanel, CVGuiScreenPanel );
	CHoloVGuiScreenPanel( Panel *parent, const char *panelname );

	virtual bool	Init( KeyValues *kv, VGuiScreenInitData_t *init );
	virtual void	OnCommand( const char *command );
	virtual vgui::Panel * CreateControlByName( const char *controlName );
};

#endif // __C_HOLO_WORLD_SCREEN_PANEL_H__