/*
===============================================================================

	holo_haptic_events.h
	Defines all haptic event types for Holodeck.

===============================================================================
*/

#ifndef __HOLO_HAPTIC_EVENTS_H__
#define __HOLO_HAPTIC_EVENTS_H__

#include "holo_haptics.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CButtonPressHapticEvent : public CHoloHapticEvent
{
public:
	CButtonPressHapticEvent();

	virtual bool	Update();

	// Accessors.
	static float	GetDuration()		{ return 0.2f; }

private:
	float			_startTime;
};

#endif // __HOLO_HAPTIC_EVENTS_H__