/*
===============================================================================

	holo_shared.h
		Code\structures that are to be shared between the client and server.

===============================================================================
*/

#ifndef __HOLO_SHARED_H__
#define __HOLO_SHARED_H__

#include <istream>

#ifdef CLIENT_DLL
	// Required for custom structure constructors.
	#include <Leap.h>
#endif

namespace holo
{

//=============================================================================
// Constants.
//=============================================================================
	enum EFinger
	{
		FINGER_THUMB,
		FINGER_POINTER,
		FINGER_MIDDLE,
		FINGER_USELESS,
		FINGER_PINKY,

		FINGER_COUNT
	};

	enum EGesture
	{
		GESTURE_CIRCLE,
		GESTURE_SWIPE,
		GESTURE_KEY_TAP,
		GESTURE_SCREEN_TAP,
		GESTURE_BALL,

		GESTURE_COUNT
	};

	enum EGlobals
	{
		INVALID_INDEX = -1
	};

//=============================================================================
// Structures.
//=============================================================================
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	struct SFinger
	{
					SFinger();
#ifdef CLIENT_DLL
					SFinger( const Leap::Finger &f );
		void		FromLeap( const Leap::Finger &f );
#endif

		int			id;
		Vector		direction;
		Vector		tipPosition;
		Vector		tipVelocity;
		float		width, length;
	};

	std::istream &operator>>( std::istream &ss, SFinger &f );

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	struct SHand
	{
					SHand();
#ifdef CLIENT_DLL
					SHand( const Leap::Hand &h );
		void		FromLeap( const Leap::Hand &h );
#endif

		int			id;
		float		confidence;
		Vector		palmPosition;
		Vector		palmVelocity;
		Vector		palmNormal;
		SFinger		fingers[FINGER_COUNT];
	};

	std::istream &operator>>( std::istream &ss, SHand &h );

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	struct SCircleGesture
	{
					SCircleGesture();
#ifdef CLIENT_DLL
					SCircleGesture( const Leap::CircleGesture &c );
		void		FromLeap( const Leap::CircleGesture &c );
#endif

		int			handId, fingerId;
		float		radius;
		Vector		center, normal;
	};

	std::istream &operator>>( std::istream &ss, SCircleGesture &c );

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	struct SSwipeGesture
	{
					SSwipeGesture();
#ifdef CLIENT_DLL
					SSwipeGesture( const Leap::SwipeGesture &s );
		void		FromLeap( const Leap::SwipeGesture &s );
#endif

		int			handId;
		float		speed;
		Vector		direction, curPosition, startPosition;
	};

	std::istream &operator>>( std::istream &ss, SSwipeGesture &s );

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	struct STapGesture
	{
					STapGesture();
#ifdef CLIENT_DLL
					STapGesture( const Leap::KeyTapGesture &k );
					STapGesture( const Leap::ScreenTapGesture &s );
		void		FromLeap( const Leap::KeyTapGesture &k );
		void		FromLeap( const Leap::ScreenTapGesture &s );
#endif

		int			handId, fingerId;
		Vector		direction, position;
	};

	std::istream &operator>>( std::istream &ss, STapGesture &t );

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	struct SBallGesture
	{
					SBallGesture();
#ifdef CLIENT_DLL
					SBallGesture( const Leap::Hand &h );
		void		FromLeap( const Leap::Hand &h );
#endif

		int			handId;
		float		radius;
		Vector		center;
	};

	std::istream &operator>>( std::istream &ss, SBallGesture &b );

//=============================================================================
// Code
//=============================================================================
#ifdef CLIENT_DLL
	Vector			LeapToHoloCoordinates( const Leap::Vector &v );

	EFinger			LeapToHoloFingerCode( const Leap::Finger::Type &finger );
#endif
	
}

#endif // __HOLO_SHARED_H__