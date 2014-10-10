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
CTriggerHapticEvent::CTriggerHapticEvent( unsigned char power, unsigned char freq ) : CHoloHapticEvent( ENVIRONMENT )
{
	_power = power;
	_frequency = freq;
	_enabled = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CTriggerHapticEvent::Update()
{
	_enabled = true;
	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CProxyHapticEvent::CProxyHapticEvent() : CHoloHapticEvent( ENVIRONMENT )
{
	_startPower = _endPower = 0;
	_startFreq = _endFreq = 0;
	_time = 0.0f;
	_lerp = ELerp::LINEAR;

	_startTime = -1.0f;
	_flags = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CProxyHapticEvent::Update()
{
	if( _flags & FL_ENABLED_BIT )
	{
		// Haptics have been activated, but lerping isn't happening.
		_enabled = true;
		_power = 255.0f * _startPower;
		_frequency = 255.0f * _startFreq;
	}
	else if( _startTime < 0.0f )
	{
		// Proxy isn't active at all.
		Clear();
	}
	else if( _startTime + _time < gpGlobals->curtime )
	{
		// Lerping has complete. Do we finish or hold?
		if( _flags & FL_HOLD_OUT_BIT )
		{
			_enabled = true;
			_power = 255.0f * _endPower;
			_frequency = 255.0f * _endFreq;
		}
		else
		{
			_startTime = -1;
			Clear();
		}
	}
	else
	{
		DoLerp();
	}

	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CProxyHapticEvent::StartLerping()
{
	_startTime = gpGlobals->curtime;
	RemoveFlag( FL_ENABLED_BIT );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CProxyHapticEvent::Enable()
{
	AddFlag( FL_ENABLED_BIT );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CProxyHapticEvent::Disable()
{
	RemoveFlag( FL_ENABLED_BIT );
	_startTime = -1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CProxyHapticEvent::DoLerp()
{
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