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
	namespace NFinger
	{
		enum type
		{
			FINGER_THUMB,
			FINGER_POINTER,
			FINGER_MIDDLE,
			FINGER_RING,
			FINGER_PINKY,

			FINGER_COUNT
		};

		const char *ToString( type finger );
	}
	typedef NFinger::type EFinger;

	namespace NGesture
	{
		enum type
		{
			GESTURE_CIRCLE,
			GESTURE_SWIPE,
			GESTURE_TAP,

			GESTURE_COUNT
		};

		const char *ToString( type gesture );
	}
	typedef NGesture::type EGesture;

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
	std::ostream &operator<<( std::ostream &ss, const SFinger &f );

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
		SFinger		fingers[EFinger::FINGER_COUNT];
	};

	std::istream &operator>>( std::istream &ss, SHand &h );
	std::ostream &operator<<( std::ostream &ss, const SHand &h );

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
	std::ostream &operator<<( std::ostream &ss, const SCircleGesture &c );

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
	std::ostream &operator<<( std::ostream &ss, const SSwipeGesture &s );

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
	std::ostream &operator<<( std::ostream &ss, const STapGesture &t );

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
		float		radius, grabStrength;
		Vector		center;
	};

	std::istream &operator>>( std::istream &ss, SBallGesture &b );
	std::ostream &operator<<( std::ostream &ss, const SBallGesture &b );

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	// THIS IS NOT THREAD SAFE!!!
	struct SFrame
	{
		friend std::istream &operator>>( std::istream &ss, SFrame &f );
		friend std::ostream &operator<<( std::ostream &ss, const SFrame &f );

						SFrame();
#ifdef CLIENT_DLL
						SFrame( const Leap::Frame &f );
		void			FromLeap( const Leap::Frame &f );
#endif

		void			Mark()				{ _marked = true; }
		bool			IsMarked()			{ return _marked; }

		bool			IsGestureActive( EGesture gesture ) const	{ return ( _gestureBits & gesture ) != 0; }
		void			SetGestureActive( EGesture gesture )		{ _gestureBits |= ( 1 << gesture ); }

#ifdef GAME_DLL
		void			ToEntitySpace( CBaseCombatCharacter *entity, const Vector &delta );
#endif

		// Frame data.
		SHand			_hand;
		SBallGesture	_ball;
		SCircleGesture	_circle;
		SSwipeGesture	_swipe;
		STapGesture		_tap;

	private:
#ifdef GAME_DLL
		void			ApplyTranslation( const Vector &offset );
		void			ApplyRotation( CBaseCombatCharacter *entity );
#endif

		int				_gestureBits;
		bool			_marked;
	};

	std::istream &operator>>( std::istream &ss, SFrame &f );
	std::ostream &operator<<( std::ostream &ss, const SFrame &f );

//=============================================================================
// Code
//=============================================================================
#ifdef CLIENT_DLL
	Vector			LeapToHoloCoordinates( const Leap::Vector &v );
	EFinger			LeapToHoloFingerCode( const Leap::Finger::Type &finger );
	float			LeapToHoloDistance( float distance );
#endif
	
}

#endif // __HOLO_SHARED_H__