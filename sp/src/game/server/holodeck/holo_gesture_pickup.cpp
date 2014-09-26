/*
===============================================================================

	grid_gesture_detector.cpp
	Detects custom player gestures.

===============================================================================
*/

#include "cbase.h"
#include "holo_gesture_detector.h"

using namespace holo;

//-----------------------------------------------------------------------------
// ConVars
//-----------------------------------------------------------------------------
static ConVar grid_pickup_strength( "grid_pickup_strength", "0.8", FCVAR_ARCHIVE );

//-----------------------------------------------------------------------------
// Gesture statics.
//-----------------------------------------------------------------------------
float CPickupGesture::_lastRadius[holo::EHand::HAND_COUNT] = { 0.0f, 0.0f };

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CPickupGesture::CPickupGesture( const CFrame &frame, EHand hand )
{ 
	_clenchState = EState::NONE; 
	Detect( frame, hand ); 
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CPickupGesture::Detect( const holo::CFrame &frame, holo::EHand hand )
{
	float grabStrength = grid_pickup_strength.GetFloat();
	float curRadius = frame.GetHand( hand ).GetBallGesture().GetGrabStrength();

	if( curRadius == 0.0f )
	{
		SetInactive();
	}
	else if( curRadius >= grabStrength && _lastRadius[hand] < grabStrength )
	{
		_clenchState = EState::STARTED;
	}
	else if( curRadius < grabStrength && _lastRadius[hand] >= grabStrength )
	{
		_clenchState = EState::FINISHED;
	}
	else if( curRadius > grabStrength )
	{
		_clenchState = EState::CLOSED;
	}

	_lastRadius[hand] = curRadius;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CPickupGesture::IsHandClenched() const
{ 
	return _clenchState == EState::CLOSED; 
}

//-----------------------------------------------------------------------------
// Was the first closed this frame?
//-----------------------------------------------------------------------------
bool CPickupGesture::HasClenchStarted() const
{ 
	return _clenchState == EState::STARTED; 
}

//-----------------------------------------------------------------------------
// Was the fist opened this frame?
//-----------------------------------------------------------------------------
bool CPickupGesture::HasClenchFinished() const
{ 
	return _clenchState == EState::FINISHED;
}