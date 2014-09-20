/*
===============================================================================

	grid_gesture_detector.h
	Detects custom player gestures.

===============================================================================
*/

#ifndef __GRID_GESTURE_DETECTOR_H__
#define __GRID_GESTURE_DETECTOR_H__

namespace grid
{

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	enum EGesture
	{
		PICKUP,
		GUN,

		COUNT
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CBaseGesture
	{
	public:
						CBaseGesture()			{ _active = true; }

		bool			IsActive() const		{ return _active; }
		void			SetInactive()			{ _active = false; }

	private:
		virtual void	Detect( const holo::CFrame &frame, holo::EHand hand ) = 0;

		bool			_active;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CPickupGesture : public CBaseGesture
	{
		enum EState
		{
			NONE,		// Gesture is not active.
			STARTED,	// Fist was closed this frame.
			CLOSED,		// Fist is closed.
			FINISHED,	// Fist was opened this frame.
		};

		friend class CGestureDetector;

	public:
						CPickupGesture()		{ _clenchState = EState::NONE; }

		bool			IsHandClenched() const	{ return _clenchState == EState::CLOSED; }

		// Was the first closed this frame?
		bool			HasClenchStarted()		{ return _clenchState == EState::STARTED; }

		// Was the fist opened this frame?
		bool			HasClenchFinished()		{ return _clenchState == EState::FINISHED; }

	private:
		virtual void	Detect( const holo::CFrame &frame, holo::EHand hand );

		EState			_clenchState;

		// Radius of the ball gesture last frame.
		static float	_lastRadius;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CGunGesture : public CBaseGesture
	{
		enum EState
		{
			NONE,		// No gun gesture is recognized.
			IDLE,		// Player has made a right angle with their pointer and thumb.
			TRIGGER,	// Player has their finger and thumb pointing in (roughly) the same direction.
		};

		friend class CGestureDetector;

	public:
						CGunGesture()			{ _state = EState::NONE; }

		bool			IsIdle() const			{ return _state == EState::IDLE; }
		bool			HoldingTrigger() const	{ return _state == EState::TRIGGER; }

	private:
		virtual void	Detect( const holo::CFrame &frame, holo::EHand hand );

		bool			DetectClosedFingers( const holo::CFrame &frame, holo::EHand hand );
		bool			DetectGangsta( const holo::CFrame &frame, holo::EHand hand );
		bool			DetectTrigger( const holo::CFrame &frame, holo::EHand hand );

		EState			_state;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CGestureDetector
	{
	public:
						CGestureDetector();

		void			SetFrame( const holo::CFrame &frame )					{ _frame = frame; }

		// Gesture status.
		void			SetGestureEnabled( EGesture gesture, bool enabled )		{ _gestureStatus[gesture] = enabled; }
		bool			IsGestureEnabled( EGesture gesture )					{ return _gestureStatus[gesture]; }

		CPickupGesture	DetectPickupGesture( holo::EHand hand );
		CGunGesture		DetectGunGesture( holo::EHand hand );

	private:
		bool			_gestureStatus[EGesture::COUNT];

		holo::CFrame	_frame;
	};

}

#endif // __GRID_GESTURE_DETECTOR_H__