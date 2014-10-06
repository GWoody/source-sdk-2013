/*
===============================================================================

	holo_world_screen.h
	Implements the base class for a panel in world space that is a child of the player.

===============================================================================
*/

#ifndef __HOLO_WORLD_SCREEN_H__
#define __HOLO_WORLD_SCREEN_H__

#include "vguiscreen.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CHoloWorldScreen : public CVGuiScreen
{
public:
	DECLARE_CLASS( CHoloWorldScreen, CVGuiScreen );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();
	CHoloWorldScreen();

	// Source entity overrides.
	virtual void	Spawn();

	// Mutators.
	void			SetType( holo::EWorldPanel type )		{ _type = type; }

	// Accessors.
	holo::EWorldPanel	GetType() const						{ return (holo::EWorldPanel)_type.Get(); }

private:
	CNetworkVar( int, _type );
};

#endif // __HOLO_WORLD_SCREEN_H__