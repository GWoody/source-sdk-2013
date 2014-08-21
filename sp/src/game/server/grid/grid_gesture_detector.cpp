/*
===============================================================================

	grid_gesture_detector.cpp
	Detects custom player gestures.

===============================================================================
*/

#include "cbase.h"
#include "grid_gesture_detector.h"

using namespace grid;

//-----------------------------------------------------------------------------
// ConVars
//-----------------------------------------------------------------------------
static ConVar grid_pickup_strength( "grid_pickup_strength", "0.8", FCVAR_ARCHIVE );

//-----------------------------------------------------------------------------
// Gesture statics.
//-----------------------------------------------------------------------------
float CPickupGesture::_lastRadius = 0.0f;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CPickupGesture::Detect( const holo::CFrame &frame )
{
	float grabStrength = grid_pickup_strength.GetFloat();
	float curRadius = frame.GetBallGesture().GetGrabStrength();

	if( curRadius >= grabStrength && _lastRadius < grabStrength )
	{
		_clenchState = EState::STARTED;
	}
	else if( curRadius < grabStrength && _lastRadius >= grabStrength )
	{
		_clenchState = EState::FINISHED;
	}
	else if( curRadius > grabStrength )
	{
		_clenchState = EState::CLOSED;
	}
	else
	{
		_clenchState = EState::NONE;
	}

	_lastRadius = curRadius;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGestureDetector::CGestureDetector()
{
	for( int i = 0; i < EGesture::COUNT; i++ )
	{
		_gestureStatus[i] = false;
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CPickupGesture CGestureDetector::DetectPickupGesture()
{
	CPickupGesture pickup;

	// Ensure we're allowed to detect this gesture.
	if( !IsGestureEnabled( EGesture::PICKUP ) )
	{
		pickup.SetInactive();
		return pickup;
	}

	pickup.Detect( _frame );
	return pickup;
}