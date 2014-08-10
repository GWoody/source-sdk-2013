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
		FINGER_RING,
		FINGER_PINKY,

		FINGER_COUNT
	};
	const char *EFingerToString( EFinger finger );

	enum EGesture
	{
		GESTURE_CIRCLE,
		GESTURE_SWIPE,
		GESTURE_TAP,

		GESTURE_COUNT
	};
	const char *EGestureToString( EGesture gesture );

	enum EGlobals
	{
		INVALID_INDEX = -1
	};
	const char *EGlobalsToString( EGlobals global );

//=============================================================================
// Structures.
//=============================================================================
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	struct SBone
	{
						SBone();
#ifdef CLIENT_DLL
						SBone(const Leap::Bone &b);
		void			FromLeap(const Leap::Bone &b);
#endif

		void			ToBitBuffer( bf_write *buf ) const;
		void			FromBitBuffer( bf_read *buf );

		void			Transform( float yaw, const Vector &translation );

		Vector			nextJoint;
		Vector			prevJoint;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	struct SFinger
	{
						SFinger();
#ifdef CLIENT_DLL
						SFinger( const Leap::Finger &f );
		void			FromLeap( const Leap::Finger &f );
#endif

		void			ToBitBuffer( bf_write *buf ) const;
		void			FromBitBuffer( bf_read *buf );

		void			Transform( float yaw, const Vector &translation );

		int				id;
		Vector			direction;
		Vector			tipPosition;
		Vector			tipVelocity;
		float			width, length;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	struct SHand
	{
						SHand();
#ifdef CLIENT_DLL
						SHand( const Leap::Hand &h );
		void			FromLeap( const Leap::Hand &h );
		void			BuildFingers( const Leap::Hand &h );
#endif

		void			ToBitBuffer( bf_write *buf ) const;
		void			FromBitBuffer( bf_read *buf );

		void			Transform( float yaw, const Vector &translation );

		int				id;
		float			confidence;
		Vector			direction, normal;
		Vector			position;
		Vector			velocity;
		SFinger			fingers[EFinger::FINGER_COUNT];
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	struct SCircleGesture
	{
						SCircleGesture();
#ifdef CLIENT_DLL
						SCircleGesture( const Leap::CircleGesture &c );
		void			FromLeap( const Leap::CircleGesture &c );
#endif

		void			ToBitBuffer( bf_write *buf ) const;
		void			FromBitBuffer( bf_read *buf );

		void			Transform( float yaw, const Vector &translation );

		int				handId, fingerId;
		float			radius, duration;
		Vector			center, normal;
		bool			clockwise;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	struct SSwipeGesture
	{
						SSwipeGesture();
#ifdef CLIENT_DLL
						SSwipeGesture( const Leap::SwipeGesture &s );
		void			FromLeap( const Leap::SwipeGesture &s );
#endif

		void			ToBitBuffer( bf_write *buf ) const;
		void			FromBitBuffer( bf_read *buf );

		void			Transform( float yaw, const Vector &translation );

		int				handId;
		float			speed;
		Vector			direction, curPosition, startPosition;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	struct STapGesture
	{
						STapGesture();
#ifdef CLIENT_DLL
						STapGesture( const Leap::KeyTapGesture &k );
						STapGesture( const Leap::ScreenTapGesture &s );
		void			FromLeap( const Leap::KeyTapGesture &k );
		void			FromLeap( const Leap::ScreenTapGesture &s );
#endif

		void			ToBitBuffer( bf_write *buf ) const;
		void			FromBitBuffer( bf_read *buf );

		void			Transform( float yaw, const Vector &translation );

		int				handId, fingerId;
		Vector			direction, position;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	struct SBallGesture
	{
						SBallGesture();
#ifdef CLIENT_DLL
						SBallGesture( const Leap::Hand &h );
		void			FromLeap( const Leap::Hand &h );
#endif

		void			ToBitBuffer( bf_write *buf ) const;
		void			FromBitBuffer( bf_read *buf );

		void			Transform( float yaw, const Vector &translation );

		int				handId;
		float			radius, grabStrength;
		Vector			center;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	// THIS IS NOT THREAD SAFE!!!
	struct SFrame
	{
						SFrame();
#ifdef CLIENT_DLL
						SFrame( const Leap::Frame &f );
		void			FromLeap( const Leap::Frame &f );
#endif

		void			ToBitBuffer( bf_write *buf ) const;
		void			FromBitBuffer( bf_read *buf );

		bool			IsGestureActive( EGesture gesture ) const	{ return ( _gestureBits & ( 1 << gesture ) ) != 0; }
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
		int				_gestureBits;
	};
	
}

#endif // __HOLO_SHARED_H__