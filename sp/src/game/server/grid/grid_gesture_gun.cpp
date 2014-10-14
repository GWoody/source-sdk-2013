/*
===============================================================================

	grid_gesture_gun.cpp
	Detects the gun gesture.

===============================================================================
*/

#include "cbase.h"
#include "grid_gesture_detector.h"
#include "holodeck/holo_shared.h"

using namespace grid;

//-----------------------------------------------------------------------------
// ConVars
//-----------------------------------------------------------------------------
static ConVar grid_gun_direction_tolerance( "grid_gun_direction_tolerance", "20", FCVAR_ARCHIVE, "Allowed variation in angles when testing the gun gesture." );
static ConVar grid_gun_idle_l_angle( "grid_gun_idle_l_angle", "50", FCVAR_ARCHIVE, "Base angle between the pointer and thumb required for the gun gesture." );
static ConVar grid_gun_trigger_angle( "grid_gun_trigger_angle", "30", FCVAR_ARCHIVE, "Upper bound to the gun gesture detecting a trigger press." );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGunGesture::CGunGesture( const CFrame &frame, EHand hand )
{ 
	_state = EState::NONE; 
	Detect( frame, hand );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CGunGesture::IsIdle() const
{ 
	return _state == EState::IDLE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CGunGesture::HoldingTrigger() const
{
	return _state == EState::TRIGGER;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGunGesture::Detect( const CFrame &frame, EHand hand )
{
	if( !frame.IsValid() )
	{
		SetInactive();
		return;
	}

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
bool CGunGesture::DetectClosedFingers( const CFrame &frame, EHand hand )
{
	float tol = grid_gun_direction_tolerance.GetFloat();
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
			return false;
		}
	}

	theta = holohand.FindThetaBetweenFingers( FINGER_RING, FINGER_PINKY );
	if( theta > tol )
	{
		return false;
	}

	// Check if the fingers are pointing the opposite direction to the pointer.
	theta = holohand.FindThetaBetweenFingers( FINGER_POINTER, FINGER_RING );
	if( theta < 90.0f )
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CGunGesture::DetectGangsta( const CFrame &frame, EHand hand )
{
	float theta = frame.GetHand(hand).FindThetaBetweenFingers( FINGER_POINTER, FINGER_THUMB );
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
bool CGunGesture::DetectTrigger( const CFrame &frame, EHand hand )
{
	const CHand &holohand = frame.GetHand( hand );
	const CFinger &pointer = holohand.GetFingerByType( FINGER_POINTER );
	const CBone &pointerBase = pointer.GetBone( BONE_INTERMEDIATE );
	const CFinger &thumb = holohand.GetFingerByType( FINGER_THUMB );

	// We want the thumb tip to be relatively close to the pointer tip.
	const Vector &diff = pointerBase.GetNextJoint() - thumb.GetTipPosition();
	if( diff.Length() > ( pointer.GetLength() ) )
	{
		return false;	
	}

	return true;
}