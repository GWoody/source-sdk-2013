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
class CTriggerHapticEvent : public CHoloHapticEvent
{
public:
	CTriggerHapticEvent( unsigned char power, unsigned char freq );

	virtual bool	Update();
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CProxyHapticEvent : public CHoloHapticEvent
{
	enum EFlags
	{
		FL_ENABLED,
		FL_HOLD_OUT,
	};

public:
	enum EFlagBit
	{
		FL_ENABLED_BIT = ( 1 << FL_ENABLED ),
		FL_HOLD_OUT_BIT = ( 1 << FL_HOLD_OUT ),
	};

	enum ELerp
	{
		LINEAR,
		EXPONENTIAL,
		SIN_CURVE,

		LERP_COUNT
	};

	CProxyHapticEvent();

	// CHoloHapticEvent overrides.
	virtual bool	Update();

	// Flags.
	void			AddFlag( EFlagBit bit )		{ _flags |= bit; }
	void			RemoveFlag( EFlagBit bit )	{ _flags &= ~bit; }

	// Mutators.
	void			SetStartPower( float p )	{ _startPower = p; }
	void			SetEndPower( float p )		{ _endPower = p; }
	void			SetStartFreq( float f )		{ _startFreq = f; }
	void			SetEndFreq( float f )		{ _endFreq = f; }
	void			SetLerpType( ELerp l )		{ _lerp = l; }
	void			SetLerpTime( float t )		{ _time = t; }
	void			SwapStartEnd()				{ V_swap( _startPower, _endPower ); V_swap( _startFreq, _endFreq ); }

	// Events.
	void			StartLerping();
	void			Enable();
	void			Disable();

private:
	typedef float(*trigFunc_t)( float f );

	void			DoLerp();
	void			DoLinearLerp( float percent );
	void			DoExpLerp( float percent );
	void			DoCurveLerp( float percent, trigFunc_t func );

	unsigned char	InternalDoExpLerp( float percent, float start, float end );
	unsigned char	InternalDoCurveLerp( float percent, float start, float end, trigFunc_t func );

	// Lerp data.
	float		_startPower, _endPower;
	float		_startFreq, _endFreq;
	ELerp		_lerp;
	float		_time;

	// State data.
	int				_flags;
	float			_startTime;
};

#endif // __HOLO_HAPTIC_EVENTS_H__