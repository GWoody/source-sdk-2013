/*
===============================================================================

	holo_haptic_events.cpp
	Implements all haptic event types for Holodeck.

===============================================================================
*/

#include "cbase.h"
#include "holo_haptic_events.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CButtonPressHapticEvent::CButtonPressHapticEvent() : CHoloHapticEvent( USE_PANEL )
{
	_startTime = gpGlobals->curtime;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CButtonPressHapticEvent::Update()
{
	if( _startTime + GetDuration() < gpGlobals->curtime )
	{
		// Event has finished.
		return false;
	}

	_power = 24;
	_frequency = 0;
	_enabled = true;

	return true;
}