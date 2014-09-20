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
static ConVar grid_gun_direction_tolerance( "grid_gun_direction_tolerance", "20", FCVAR_ARCHIVE, "Allowed variation in angles when testing the gun gesture." );
static ConVar grid_gun_idle_l_angle( "grid_gun_idle_l_angle", "50", FCVAR_ARCHIVE, "Base angle between the pointer and thumb required for the gun gesture." );
static ConVar grid_gun_trigger_angle( "grid_gun_trigger_angle", "30", FCVAR_ARCHIVE, "Upper bound to the gun gesture detecting a trigger press." );

//-----------------------------------------------------------------------------
// Gesture statics.
//-----------------------------------------------------------------------------
float CPickupGesture::_lastRadius = 0.0f;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CPickupGesture::Detect( const holo::CFrame &frame, holo::EHand hand )
{
	float grabStrength = grid_pickup_strength.GetFloat();
	float curRadius = frame.GetHand( hand ).GetBallGesture().GetGrabStrength();

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
		SetInactive();
	}

	_lastRadius = curRadius;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGunGesture::Detect( const holo::CFrame &frame, holo::EHand hand )
{
	if( !DetectClosedFingers( frame, hand ) )
	{
		SetInactive();
	}
	else if( DetectTrigger( frame, hand ) )
	{
		_state = EState::TRIGGER;
	}
	else if( DetectGangsta( frame, hand ) )
	{
		_state = EState::IDLE;
	}
}

//-----------------------------------------------------------------------------
// Ensure the middle, ring and pinky fingers are all pointing the same direction.
//-----------------------------------------------------------------------------
bool CGunGesture::DetectClosedFingers( const holo::CFrame &frame, holo::EHand hand )
{
	float tol = grid_gun_direction_tolerance.GetFloat();
	float theta;

	// Occasionally the middle finger gets "stuck" to the pointer and will copy its direction.
	theta = frame.GetHand(hand).FindThetaBetweenFingers( holo::EFinger::FINGER_POINTER, holo::EFinger::FINGER_MIDDLE );
	if( theta > tol )
	{
		// The middle finger is not stuck. Ensure the ring and middle fingers are pointing in roughly the same direction.
		theta = frame.GetHand(hand).FindThetaBetweenFingers( holo::EFinger::FINGER_RING, holo::EFinger::FINGER_MIDDLE );
		if( theta > tol )
		{
			return false;
		}
	}

	theta = frame.GetHand(hand).FindThetaBetweenFingers( holo::EFinger::FINGER_RING, holo::EFinger::FINGER_PINKY );
	if( theta > tol )
	{
		return false;
	}

	// Check if the fingers are pointing the opposite direction to the pointer.
	theta = frame.GetHand(hand).FindThetaBetweenFingers( holo::EFinger::FINGER_POINTER, holo::EFinger::FINGER_RING );
	if( theta < 90.0f )
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CGunGesture::DetectGangsta( const holo::CFrame &frame, holo::EHand hand )
{
	float theta = frame.GetHand(hand).FindThetaBetweenFingers( holo::EFinger::FINGER_POINTER, holo::EFinger::FINGER_THUMB );
	float baseAngle = grid_gun_idle_l_angle.GetFloat();
	float tol = grid_gun_direction_tolerance.GetFloat();

	if( theta < baseAngle - tol || theta > baseAngle + tol )
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CGunGesture::DetectTrigger( const holo::CFrame &frame, holo::EHand hand )
{
	const holo::CFinger &pointer = frame.GetHand(hand).GetFingerByType( holo::EFinger::FINGER_POINTER );
	const holo::CBone &pointerBase = pointer.GetBone( holo::EBone::BONE_INTERMEDIATE );
	const holo::CFinger &thumb = frame.GetHand(hand).GetFingerByType( holo::EFinger::FINGER_THUMB );

	// We want the thumb tip to be relatively close to the pointer tip.
	const Vector &diff = pointerBase.GetNextJoint() - thumb.GetTipPosition();
	if( diff.Length() > ( pointer.GetLength() ) )
	{
		return false;	
	}

	return true;
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
CPickupGesture CGestureDetector::DetectPickupGesture( holo::EHand hand )
{
	CPickupGesture pickup;

	// Ensure we're allowed to detect this gesture.
	if( !IsGestureEnabled( EGesture::PICKUP ) )
	{
		pickup.SetInactive();
		return pickup;
	}

	pickup.Detect( _frame, hand );
	return pickup;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGunGesture CGestureDetector::DetectGunGesture( holo::EHand hand )
{
	CGunGesture gun;

	// Ensure we're allowed to detect this gesture.
	if( !IsGestureEnabled( EGesture::GUN ) )
	{
		gun.SetInactive();
		return gun;
	}

	gun.Detect( _frame, hand );
	return gun;
}