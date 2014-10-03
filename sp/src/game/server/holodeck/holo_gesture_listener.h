/*
===============================================================================

	holo_gesture_listener.h
	Implements a map entity that fires callbacks whenever a gesture has
	been performed in a map.

===============================================================================
*/

#ifndef __HOLO_GESTURE_LISTENER_H__
#define __HOLO_GESTURE_LISTENER_H__

#include "holodeck/holo_shared.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CHoloGestureCallback : public CLogicalEntity
{
public:
	virtual void	OnCircleGesture( const holo::CFrame &frame, const holo::CCircleGesture &circle )	{}
	virtual void	OnSwipeGesture( const holo::CFrame &frame, const holo::CSwipeGesture &swipe )		{}
	virtual void	OnTapGesture( const holo::CFrame &frame, const holo::CTapGesture &tap )				{}
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CHoloGestureListener : public CLogicalEntity
{
public:
	DECLARE_CLASS( CHoloGestureListener, CLogicalEntity );
	DECLARE_DATADESC();

	// CBaseEntity overrides.
	virtual void	Spawn();
	
	// Gesture processing.
	void			OnCircleGesture( const holo::CFrame &frame,const holo::CCircleGesture &circle );
	void			OnSwipeGesture( const holo::CFrame &frame, const holo::CSwipeGesture &swipe );
	void			OnTapGesture( const holo::CFrame &frame, const holo::CTapGesture &tap );

	// Singleton stuff.
	static void		ClearInstance()				{ _instance = NULL; }
	static CHoloGestureListener *	Get()		{ return _instance; }

private:
	// We only need 1 listener per map.
	static CHoloGestureListener *	_instance;

	// State.
	EHANDLE			_callbacks[holo::EGesture::GESTURE_COUNT];

	// Hammer inputs.
	void			InputSetCircleGestureListener( inputdata_t &input );
	void			InputSetSwipeGestureListener( inputdata_t &input );
	void			InputSetTapGestureListener( inputdata_t &input );
	void			InputEnable( inputdata_t &input );
	void			InputDisable( inputdata_t &input );

	// Hammer outputs.
	COutputEvent	_onCircleGesture, _onSwipeGesture, _onTapGesture;

	// Hammer attributes.
	bool			_disabled;
	string_t		_circleTarget, _swipeTarget, _tapTarget;
};

#endif // __HOLO_GESTURE_LISTENER_H__;