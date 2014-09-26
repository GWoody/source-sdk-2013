/*
===============================================================================

	grid_gesture_detector.h
	Defines custom player gestures.

===============================================================================
*/

#ifndef __GRID_GESTURE_DETECTOR_H__
#define __GRID_GESTURE_DETECTOR_H__

#include "holodeck/holo_gesture_detector.h"

namespace grid
{

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CGunGesture : public holo::CBaseGesture
	{
		enum EState
		{
			NONE,		// No gun gesture is recognized.
			IDLE,		// Player has made a right angle with their pointer and thumb.
			TRIGGER,	// Player has their finger and thumb pointing in (roughly) the same direction.
		};

	public:
						CGunGesture( const holo::CFrame &frame, holo::EHand hand );

		bool			IsIdle() const;
		bool			HoldingTrigger() const;

	protected:
		virtual void	Detect( const holo::CFrame &frame, holo::EHand hand );

	private:
		bool			DetectClosedFingers( const holo::CFrame &frame, holo::EHand hand );
		bool			DetectGangsta( const holo::CFrame &frame, holo::EHand hand );
		bool			DetectTrigger( const holo::CFrame &frame, holo::EHand hand );

		EState			_state;
	};

}

#endif // __GRID_GESTURE_DETECTOR_H__