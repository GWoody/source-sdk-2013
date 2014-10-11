/*
===============================================================================

	holo_world_screen_manager.h
	Handles the 3 VGUI panels that are children of a player.

===============================================================================
*/

#ifndef __HOLO_WORLD_SCREEN_MANAGER_H__
#define __HOLO_WORLD_SCREEN_MANAGER_H__

#include "GameEventListener.h"

class CHoloWorldScreen;
class CHoloPlayer;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CHoloWorldScreenManager : public CGameEventListener
{
public:
	CHoloWorldScreenManager( CHoloPlayer *owner );

	// CGameEventListener implementation.
	virtual void	FireGameEvent( IGameEvent *event );

	virtual bool	CreateScreen( EWorldPanel panel, const char *typeName );
	virtual bool	IsScreenActive( EWorldPanel panel )		{ return GetScreen(panel) != NULL; }
	virtual void	DestroyScreen( EWorldPanel panel );

	CHoloWorldScreen *	GetScreen( EWorldPanel panel ) const	{ return _screens[panel]; }

private:
	CHoloWorldScreen *	_screens[WORLD_PANEL_COUNT];
	CHoloPlayer *	_owner;
};

#endif // __HOLO_WORLD_SCREEN_MANAGER_H__