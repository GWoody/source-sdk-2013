/*
===============================================================================

	holo_screen_proxy.cpp
	Allows map designers to bind screens to gestures.

===============================================================================
*/

#include "cbase.h"
#include "holo_player.h"

#include "holodeck/holo_shared.h"
using namespace holo;

string_t global_screen_gesture_screen;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CHoloScreenProxy : public CLogicalEntity
{
public:
	DECLARE_CLASS( CHoloScreenProxy, CLogicalEntity );
	DECLARE_DATADESC();

	// Source entity overrides.
	virtual void	Activate();

private:
	// Hammer inputs.
	void			InputActivateLeftScreen( inputdata_t &data );
	void			InputActivateMiddleScreen( inputdata_t &data );
	void			InputActivateRightScreen( inputdata_t &data );
	void			InputDestroyLeftScreen( inputdata_t &data );
	void			InputDestroyMiddleScreen( inputdata_t &data );
	void			InputDestroyRightScreen( inputdata_t &data );
	void			InputSetScreenGestureScreen( inputdata_t &data );

	// Hammer attributes.
	string_t		_screenGestureScreen;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( holo_screen_proxy, CHoloScreenProxy );

BEGIN_DATADESC( CHoloScreenProxy )

	// Attributes.
	DEFINE_KEYFIELD( _screenGestureScreen, FIELD_STRING, "screenGestureScreen" ),

	// Inputs.
	DEFINE_INPUTFUNC( FIELD_STRING, "ActivateLeftScreen", InputActivateLeftScreen ),
	DEFINE_INPUTFUNC( FIELD_STRING, "ActivateMiddleScreen", InputActivateMiddleScreen ),
	DEFINE_INPUTFUNC( FIELD_STRING, "ActivateRightScreen", InputActivateRightScreen ),
	DEFINE_INPUTFUNC( FIELD_VOID, "DestroyLeftScreen", InputDestroyLeftScreen ),
	DEFINE_INPUTFUNC( FIELD_VOID, "DestroyMiddleScreen", InputDestroyMiddleScreen ),
	DEFINE_INPUTFUNC( FIELD_VOID, "DestroyRightScreen", InputDestroyRightScreen ),
	DEFINE_INPUTFUNC( FIELD_STRING, "SetScreenGestureScreen", InputSetScreenGestureScreen ),

END_DATADESC()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloScreenProxy::Activate()
{
	global_screen_gesture_screen = _screenGestureScreen;

	BaseClass::Activate();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloScreenProxy::InputActivateLeftScreen( inputdata_t &data )
{
	CHoloPlayer *player = (CHoloPlayer *)UTIL_GetLocalPlayer();
	player->GetScreenManager().CreateScreen( WORLD_PANEL_LEFT, data.value.String() );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloScreenProxy::InputActivateMiddleScreen( inputdata_t &data )
{
	CHoloPlayer *player = (CHoloPlayer *)UTIL_GetLocalPlayer();
	player->GetScreenManager().CreateScreen( WORLD_PANEL_MIDDLE, data.value.String() );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloScreenProxy::InputActivateRightScreen( inputdata_t &data )
{
	CHoloPlayer *player = (CHoloPlayer *)UTIL_GetLocalPlayer();
	player->GetScreenManager().CreateScreen( WORLD_PANEL_RIGHT, data.value.String() );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloScreenProxy::InputDestroyLeftScreen( inputdata_t &data )
{
	CHoloPlayer *player = (CHoloPlayer *)UTIL_GetLocalPlayer();
	player->GetScreenManager().DestroyScreen( WORLD_PANEL_LEFT );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloScreenProxy::InputDestroyMiddleScreen( inputdata_t &data )
{
	CHoloPlayer *player = (CHoloPlayer *)UTIL_GetLocalPlayer();
	player->GetScreenManager().DestroyScreen( WORLD_PANEL_MIDDLE );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloScreenProxy::InputDestroyRightScreen( inputdata_t &data )
{
	CHoloPlayer *player = (CHoloPlayer *)UTIL_GetLocalPlayer();
	player->GetScreenManager().DestroyScreen( WORLD_PANEL_RIGHT );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloScreenProxy::InputSetScreenGestureScreen( inputdata_t &data )
{
	global_screen_gesture_screen = AllocPooledString( data.value.String() );
}