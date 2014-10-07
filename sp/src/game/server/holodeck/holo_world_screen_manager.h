/*
===============================================================================

	holo_world_screen_manager.h
	Handles the 3 VGUI panels that are children of a player.

===============================================================================
*/

#ifndef __HOLO_WORLD_SCREEN_MANAGER_H__
#define __HOLO_WORLD_SCREEN_MANAGER_H__

class CHoloWorldScreen;
class CHoloPlayer;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CHoloWorldScreenManager
{
public:
	CHoloWorldScreenManager();

	virtual bool	CreateScreen( EWorldPanel panel, const char *typeName, CHoloPlayer *owner );
	virtual bool	IsScreenActive( EWorldPanel panel )		{ return GetScreen(panel) != NULL; }
	virtual void	DestroyScreen( EWorldPanel panel );

	CHoloWorldScreen *	GetScreen( EWorldPanel panel ) const	{ return _screens[panel]; }

private:
	CHoloWorldScreen *	_screens[WORLD_PANEL_COUNT];
};

#endif // __HOLO_WORLD_SCREEN_MANAGER_H__