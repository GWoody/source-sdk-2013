/*
===============================================================================

	grid_usermessages.cpp
	Registers all server->client messages.

===============================================================================
*/

#include "cbase.h"
#include "usermessages.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void RegisterUserMessages( void )
{
	usermessages->Register( "Geiger", 1 );
	usermessages->Register( "ResetHUD", 1 );	// called every respawn
	usermessages->Register( "Damage", 18 );		// BUG: floats are sent for coords, no variable bitfields in hud & fixed size Msg
}