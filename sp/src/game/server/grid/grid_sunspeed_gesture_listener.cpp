/*
===============================================================================

	grid_sunspeed_gesture_listener.cpp
	Waits for the circle gesture to change the speed of the sun (in the Eden demo).

===============================================================================
*/

#include "cbase.h"
#include "holodeck/holo_gesture_listener.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
extern ConVar grid_sun_speed_multiplier;
static ConVar grid_sun_speed_gesture_history( "grid_sun_speed_gesture_history", "10", FCVAR_ARCHIVE );
static ConVar grid_sun_speed_gesture_multipler( "grid_sun_speed_gesture_multipler", "3", FCVAR_ARCHIVE );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridSunSpeedListener : public CHoloGestureCallback
{
public:
	DECLARE_CLASS( CGridSunSpeedListener, CHoloGestureCallback );

	virtual void	OnCircleGesture( const CFrame &frame, const CCircleGesture &circle );

private:
	float			AverageSpeed( float newSpeed );

	CUtlVector<float>	_history;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( grid_sunspeed_gesture_listener, CGridSunSpeedListener );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridSunSpeedListener::OnCircleGesture( const CFrame &frame, const CCircleGesture &circle )
{
	if( circle.GetProgress() < 1.0f )
	{
		// Make the circle gesture less sensitive than the standard gesture panels.
		return;
	}

	const CHand &hand = frame.GetHand( circle.GetHandType() );
	const CFinger &finger = hand.GetFingerByType( circle.GetFingerType() );

	// Clockwise puts the sun in reverse.
	float currentSpeed = finger.GetTipVelocity().Length();
	currentSpeed = circle.IsClockwise() ? -currentSpeed : currentSpeed;

	float speed = AverageSpeed( currentSpeed ) * grid_sun_speed_gesture_multipler.GetFloat();
	grid_sun_speed_multiplier.SetValue( speed );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float CGridSunSpeedListener::AverageSpeed( float newSpeed )
{
	if( _history.Count() >= grid_sun_speed_gesture_history.GetInt() )
	{
		// Ensure we have `grid_sun_speed_gesture_history - 1` elements.
		_history.RemoveMultipleFromTail( _history.Count() - grid_sun_speed_gesture_history.GetInt() + 1 );
	}

	_history.AddToHead( newSpeed );

	float avg = 0.0f;
	for( int i = 0; i < _history.Count(); i++ )
	{
		avg += _history[i];
	}

	return avg / _history.Count();
}