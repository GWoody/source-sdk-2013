/*
==============================================================================

	in_move.h
	Implements the interface to the PS Move headtracking system. 

==============================================================================
*/

#include "cbase.h"
#include "in_move.h"

#include "psmove.h"
#include "psmove_tracker.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CON_COMMAND( holo_start_psmove_tracker, "Begins Move calibration." )
{
	CPlaystationMove::Get().StartTracker();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CPlaystationMove *CPlaystationMove::_instance;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CPlaystationMove::CPlaystationMove()
{
	_move = NULL;
	_tracker = NULL;
	_calibrated = false;

	if( psmove_init( PSMOVE_CURRENT_VERSION ) == PSMove_False )
	{
		ConColorMsg( COLOR_YELLOW, "Failed to init PS Move API\n" );
		return;
	}

	ConColorMsg( COLOR_GREEN, __FUNCTION__": Found %d controllers.\n", psmove_count_connected() );

	// Connect to a device.
	for( int i = 0; i < psmove_count_connected(); i++ )
	{
		ConColorMsg( COLOR_GREEN, __FUNCTION__": Connecting to device %d.\n", i );
		_move = psmove_connect_by_id( i );
		if( !_move )
		{
			ConColorMsg( COLOR_YELLOW, __FUNCTION__": Failed to connect!\n" );
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CPlaystationMove::~CPlaystationMove()
{
	if( _move )
	{
		psmove_disconnect( _move );
	}

	if( _tracker )
	{
		psmove_tracker_free( _tracker );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CPlaystationMove::CreateMove( CUserCmd *cmd )
{
	if( !_move || !_tracker || !_calibrated )
	{
		return;
	}

	Vector pos;
	float radius;

	psmove_tracker_update( _tracker, NULL );
	psmove_tracker_get_position( _tracker, _move, &pos.x, &pos.y, &radius );
	pos.z = psmove_tracker_distance_from_radius( _tracker, radius );

	ConColorMsg( COLOR_GREEN, __FUNCTION__": ( %.1f, %.1f, %.1f )\n", pos.x, pos.y, pos.z );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CPlaystationMove::StartTracker()
{
	if( _move && !_calibrated )
	{
		if( !_tracker )
		{
			ConColorMsg( COLOR_GREEN, __FUNCTION__": Creating new tracker.\n" );
			_tracker = psmove_tracker_new();
		}
		
		if( _tracker )
		{
			ConColorMsg( COLOR_GREEN, __FUNCTION__": Beginning calibration.\n" );
			int result = psmove_tracker_enable( _tracker, _move );

			if( result == Tracker_CALIBRATED )
			{
				ConColorMsg( COLOR_GREEN, __FUNCTION__": Succeeded.\n" );
				_calibrated = true;
			}
			else
			{
				ConColorMsg( COLOR_YELLOW, __FUNCTION__": Failed.\n" );
			}
		}
	}
}