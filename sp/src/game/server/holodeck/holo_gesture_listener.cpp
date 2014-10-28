/*
===============================================================================

	holo_gesture_listener.cpp
	Implements a map entity that fires callbacks whenever a gesture has
	been performed in a map.

===============================================================================
*/

#include "cbase.h"
#include "holo_gesture_listener.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( holo_gesture_listener, CHoloGestureListener );

BEGIN_DATADESC( CHoloGestureListener )

	// Inputs.
	DEFINE_INPUTFUNC( FIELD_STRING, "SetCircleGestureListener", InputSetCircleGestureListener ),
	DEFINE_INPUTFUNC( FIELD_STRING, "SetSwipeGestureListener", InputSetSwipeGestureListener ),
	DEFINE_INPUTFUNC( FIELD_STRING, "SetTapGestureListener", InputSetTapGestureListener ),

	// Outputs.
	DEFINE_OUTPUT( _onCircleGesture, "OnCircleGesture" ),
	DEFINE_OUTPUT( _onSwipeGesture, "OnSwipeGesture" ),
	DEFINE_OUTPUT( _onTapGesture, "OnTapGesture" ),
	
	// Attributes.
	DEFINE_KEYFIELD( _disabled, FIELD_BOOLEAN, "StartDisabled" ),
	DEFINE_KEYFIELD( _circleTarget, FIELD_STRING, "circleTarget" ),
	DEFINE_KEYFIELD( _swipeTarget, FIELD_STRING, "swipeTarget" ),
	DEFINE_KEYFIELD( _tapTarget, FIELD_STRING, "tapTarget" ),

END_DATADESC()

CHoloGestureListener *CHoloGestureListener::_instance = NULL;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloGestureListener::Spawn()
{
	if( _instance )
	{
		Assert( 0 );
		ConColorMsg( COLOR_YELLOW, __FUNCTION__": More than one \"holo_gesture_listener\" has been spawned!\n" );
	}

	//
	// Set initial state.
	//
	_instance = this;
	_callbacks[EGesture::GESTURE_CIRCLE] = gEntList.FindEntityByName( NULL, _circleTarget );
	_callbacks[EGesture::GESTURE_SWIPE] = gEntList.FindEntityByName( NULL, _swipeTarget );
	_callbacks[EGesture::GESTURE_TAP] = gEntList.FindEntityByName( NULL, _tapTarget );

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloGestureListener::OnCircleGesture( const CFrame &frame, const CCircleGesture &circle )
{
	if( _disabled )
	{
		return;
	}

	CHoloGestureCallback *callback = dynamic_cast<CHoloGestureCallback *>( _callbacks[EGesture::GESTURE_CIRCLE].Get() );
	if( !callback )
	{
		return;
	}

	callback->OnCircleGesture( frame, circle );
	_onCircleGesture.FireOutput( UTIL_GetLocalPlayer(), this );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloGestureListener::OnSwipeGesture( const CFrame &frame, const CSwipeGesture &swipe )
{
	if( _disabled )
	{
		return;
	}

	CHoloGestureCallback *callback = dynamic_cast<CHoloGestureCallback *>( _callbacks[EGesture::GESTURE_SWIPE].Get() );
	if( !callback )
	{
		return;
	}

	callback->OnSwipeGesture( frame, swipe );
	_onSwipeGesture.FireOutput( UTIL_GetLocalPlayer(), this );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloGestureListener::OnTapGesture( const CFrame &frame, const CTapGesture &tap )
{
	if( _disabled )
	{
		return;
	}

	CHoloGestureCallback *callback = dynamic_cast<CHoloGestureCallback *>( _callbacks[EGesture::GESTURE_TAP].Get() );
	if( !callback )
	{
		return;
	}

	callback->OnTapGesture( frame, tap );
	_onTapGesture.FireOutput( UTIL_GetLocalPlayer(), this );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloGestureListener::InputSetCircleGestureListener( inputdata_t &input )
{
	_callbacks[EGesture::GESTURE_CIRCLE] = gEntList.FindEntityByName( NULL, input.value.StringID() );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloGestureListener::InputSetSwipeGestureListener( inputdata_t &input )
{
	_callbacks[EGesture::GESTURE_SWIPE] = gEntList.FindEntityByName( NULL, input.value.StringID() );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloGestureListener::InputSetTapGestureListener( inputdata_t &input )
{
	_callbacks[EGesture::GESTURE_TAP] = gEntList.FindEntityByName( NULL, input.value.StringID() );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloGestureListener::InputEnable( inputdata_t &input )
{
	_disabled = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloGestureListener::InputDisable( inputdata_t &input )
{
	_disabled = true;
}