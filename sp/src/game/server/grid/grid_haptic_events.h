/*
===============================================================================

	grid_haptic_events.h
	Defines all haptic event types for Grid.

===============================================================================
*/

#ifndef __GRID_HAPTIC_EVENTS_H__
#define __GRID_HAPTIC_EVENTS_H__

#include "holodeck/holo_haptics.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CButtonPressHapticEvent : public CHoloHapticEvent
{
public:
	CButtonPressHapticEvent();

	virtual bool	Update();

	// Accessors.
	static float	GetDuration()		{ return 0.1f; }

private:
	float			_startTime;
};

#endif // __GRID_HAPTIC_EVENTS_H__