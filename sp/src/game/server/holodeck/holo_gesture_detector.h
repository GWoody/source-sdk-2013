/*
===============================================================================

	holo_gesture_detector.h
	Detects custom player gestures.

===============================================================================
*/

#ifndef __HOLO_GESTURE_DETECTOR_H__
#define __HOLO_GESTURE_DETECTOR_H__

namespace holo
{

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

	public:
						CPickupGesture( const holo::CFrame &frame, holo::EHand hand );

		bool			IsHandClenched() const;

		// Was the first closed this frame?
		bool			HasClenchStarted() const;

		// Was the fist opened this frame?
		bool			HasClenchFinished() const;

	protected:
		virtual void	Detect( const holo::CFrame &frame, holo::EHand hand );

	private:
		EState			_clenchState;

		// Radius of the ball gesture last frame.
		static float	_lastRadius[holo::EHand::HAND_COUNT];
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CScreenGesture : public CBaseGesture
	{
	public:
						CScreenGesture( const holo::CFrame &frame, holo::EHand hand );

	protected:
		virtual void	Detect( const holo::CFrame &frame, holo::EHand hand );
	};

}

#endif // __HOLO_GESTURE_DETECTOR_H__