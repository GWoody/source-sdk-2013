/*
===============================================================================

	base_clientmodenormal.cpp
	Draws the Grid hud.

===============================================================================
*/

#ifndef __GRID_CLIENTMODENORMAL_H__
#define __GRID_CLIENTMODENORMAL_H__

#include "clientmode_shared.h"
#include <vgui_controls/EditablePanel.h>
#include <vgui/Cursor.h>

class CHudViewport;

namespace vgui
{
	typedef unsigned long HScheme;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class ClientModeGridNormal : public ClientModeShared
{
public:
	DECLARE_CLASS( ClientModeGridNormal, ClientModeShared );

	ClientModeGridNormal();
	~ClientModeGridNormal();

	virtual void	Init();
	virtual bool	ShouldDrawCrosshair( void );
};

extern IClientMode *GetClientModeNormal();
extern vgui::HScheme g_hVGuiCombineScheme;

#endif // __GRID_CLIENTMODENORMAL_H__
