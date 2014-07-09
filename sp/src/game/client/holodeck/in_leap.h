#ifndef __IN_LEAP_H__
#define __IN_LEAP_H__

#include <tier0/threadtools.h>

#include <Leap.h> 
#include <queue>
#include <string>
#include <sstream>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

class SFrameQueue
{
	public:
		void							pushOnToQueue( std::string );
		std::string						popOffQueue();
		bool							isEmpty();
	private:
		std::queue<std::string>			_frameQueue;
		CThreadMutex					_mutex;
};


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

class CLeapMotion
{
	public:
		enum typeTag_e
		{
			L_VECTOR,
			L_HAND,
			L_FINGER,
			L_CIRCLE,
			L_SWIPE,
			L_KTAP,
			L_STAP,
			L_BALL

		};
		CLeapMotion();
		~CLeapMotion();
	private:
		Leap::Controller				_controller;
		class CLeapMotionListener *		_pListener;
		CThreadMutex					_mutex;



};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

class CLeapMotionListener : public Leap::Listener
{
	public:
		CLeapMotionListener( CLeapMotion* pLeap );

		virtual void					onConnect( const Leap::Controller & controller );
		virtual void					onFrame( const Leap::Controller &controller );
		std::string						CircleGestureToString( const Leap::CircleGesture &circleGesture );
		std::string						KeyTapGestureToString ( const Leap::KeyTapGesture &keyTapGesture );
		std::string						ScreenTapGestureToString( const Leap::ScreenTapGesture &ScreenTapGesture );
		std::string						SwipeGestureToString( const Leap::SwipeGesture &swipeGesture );
		std::string						BallGestureToString(const Leap::Hand &hand ); 
		std::string						FingerToString( const Leap::Finger &finger );
		std::string						HandToString( const Leap::Hand &hand );
		std::string						LeapVectorToString( Leap::Vector &vector );
		std::string						HeaderToString(const Leap::Frame &frame, CLeapMotion::typeTag_e enumeration );

	private:
		CLeapMotion *					_pLeap;
};


#endif // __IN_LEAP_H__