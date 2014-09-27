/*
===============================================================================

	holo_haptic_events.cpp
	Implements all haptic event types for Holodeck.

===============================================================================
*/

#include "cbase.h"
#include "holo_haptic_events.h"

#include <math.h>

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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CProxyHapticEvent::CProxyHapticEvent( float startPower, float endPower, float startFreq, float endFreq, float lerpTime, ELerp lerp ) : CHoloHapticEvent( ENVIRONMENT ),
	_startPower( startPower ), _endPower( endPower ),
	_startFreq( startFreq ), _endFreq( endFreq ),
	_time( lerpTime ), _lerp( lerp )
{
	_startTime = gpGlobals->curtime;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CProxyHapticEvent::Update()
{
	if( _startTime + _time < gpGlobals->curtime )
	{
		return false;
	}

	_enabled = true;
	float percent = ( gpGlobals->curtime - _startTime ) / _time;

	switch( _lerp )
	{
		case ELerp::LINEAR:
			DoLinearLerp( percent );
			break;

		case ELerp::EXPONENTIAL:
			DoExpLerp( percent );
			break;

		case ELerp::SIN_CURVE:
			DoCurveLerp( percent, sin );
			break;
	}

	return true;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CProxyHapticEvent::DoLinearLerp( float percent )
{
	_power = 255.0f * Lerp( percent, _startPower, _endPower );
	_frequency = 255.0f * Lerp( percent, _startFreq, _endFreq );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CProxyHapticEvent::DoExpLerp( float percent )
{
	_power = InternalDoExpLerp( percent, _startPower, _endPower );
	_frequency = InternalDoExpLerp( percent, _startFreq, _endFreq );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
unsigned char CProxyHapticEvent::InternalDoExpLerp( float percent, float start, float end )
{
	float percentSqr = ( percent * percent );
	float delta = end - start;
	return 255.0f * ( start + ( delta * percentSqr ) );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CProxyHapticEvent::DoCurveLerp( float percent, trigFunc_t func )
{
	_power = InternalDoCurveLerp( percent, _startPower, _endPower, func );
	_frequency = InternalDoCurveLerp( percent, _startFreq, _endFreq, func );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
unsigned char CProxyHapticEvent::InternalDoCurveLerp( float percent, float start, float end, trigFunc_t func )
{	
	float delta = end - start;
	float val = func( percent * M_PI_F );
	return 255.0f * ( start + ( delta * val ) );
}