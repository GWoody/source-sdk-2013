/*
===============================================================================

	holo_gesture_screen.cpp
	Detects the screen gesture.

===============================================================================
*/

#include "cbase.h"
#include "holo_gesture_detector.h"
#include "holodeck/holo_shared.h"

using namespace holo;

static ConVar holo_screen_gesture_tolerance( "holo_screen_gesture_tolerance", "20", FCVAR_ARCHIVE, "Allowed variation in angles when testing the screen gesture." );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CScreenGesture::CScreenGesture( const CFrame &frame, EHand hand )
{ 
	Detect( frame, hand ); 
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CScreenGesture::Detect( const CFrame &frame, EHand hand )
{
	float tol = holo_screen_gesture_tolerance.GetFloat();
	float theta;

	const CHand &holohand = frame.GetHand( hand );

	// Occasionally the middle finger gets "stuck" to the pointer and will copy its direction.
	theta = holohand.FindThetaBetweenFingers( FINGER_POINTER, FINGER_MIDDLE );
	if( theta > tol )
	{
		// The middle finger is not stuck. Ensure the ring and middle fingers are pointing in roughly the same direction.
		theta = holohand.FindThetaBetweenFingers( FINGER_RING, FINGER_MIDDLE );
		if( theta > tol )
		{
			SetInactive();
			return;
		}
	}

	theta = holohand.FindThetaBetweenFingers( FINGER_RING, FINGER_PINKY );
	if( theta < 65.0f - tol )
	{
		SetInactive();
		return;
	}

	// Check if the fingers are pointing the opposite direction to the pointer.
	theta = holohand.FindThetaBetweenFingers( FINGER_POINTER, FINGER_RING );
	if( theta < 65.0f - tol )
	{
		SetInactive();
		return;
	}
}