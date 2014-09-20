/*
==============================================================================

	in_move.h
	Implements the interface to the PS Move headtracking system. 

==============================================================================
*/

#include "cbase.h"
#include "in_move.h"

#include "psmove.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CPlaystationMove *CPlaystationMove::_instance;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CPlaystationMove::CPlaystationMove()
{
	if( psmove_init( PSMOVE_CURRENT_VERSION ) == PSMove_False )
	{
		ConColorMsg( COLOR_YELLOW, "Failed to init PS Move API\n" );
		return;
	}

	ConColorMsg( COLOR_GREEN, "PS Move API found %d controllers.\n", psmove_count_connected() );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CPlaystationMove::~CPlaystationMove()
{

}