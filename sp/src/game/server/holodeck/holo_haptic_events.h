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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CProxyHapticEvent : public CHoloHapticEvent
{
public:
	enum ELerp
	{
		LINEAR,
		EXPONENTIAL,
		SIN_CURVE,

		LERP_COUNT
	};

	CProxyHapticEvent( float startPower, float endPower, float startFreq, float endFreq, float lerpTime, ELerp lerp );

	virtual bool	Update();

private:
	typedef float(*trigFunc_t)( float f );

	void			DoLinearLerp( float percent );
	void			DoExpLerp( float percent );
	void			DoCurveLerp( float percent, trigFunc_t func );

	unsigned char	InternalDoExpLerp( float percent, float start, float end );
	unsigned char	InternalDoCurveLerp( float percent, float start, float end, trigFunc_t func );

	float			_startPower, _endPower;
	float			_startFreq, _endFreq;
	ELerp			_lerp;
	float			_time;

	float			_startTime;
};

#endif // __HOLO_HAPTIC_EVENTS_H__