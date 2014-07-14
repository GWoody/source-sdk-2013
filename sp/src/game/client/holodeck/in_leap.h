#ifndef __IN_LEAP_H__
#define __IN_LEAP_H__

#include <tier0/threadtools.h>

#include <Leap.h> 
#include <queue>
#include <string>
#include <sstream>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

// THIS IS NOT THREAD SAFE!!!
struct SLeapFrame
{
					SLeapFrame()		{ marked = false; }
	void			mark()				{ marked = true; }
	bool			isMarked()			{ return marked; }

	// Frame data.
	bool			isEmpty();
	void			push( const std::string &str );
	std::string		pop();

private:
	bool			marked;
	std::queue<std::string>	data;		// Hand, finger and gesture data for this frame.
};

// This is thread safe.
class SFrameQueue
{
	public:
		SFrameQueue()					{ _instance = NULL; }

		// Singleton methods.
		static SFrameQueue &			get()		{ return *_instance; }
		static void						create()	{ _instance = new SFrameQueue; }
		static void						destroy()	{ delete _instance; }

		void							pushOnToQueue( const SLeapFrame &frame );
		SLeapFrame						popOffQueue();
		SLeapFrame 						peek();
		bool							isEmpty();

		void							markLast()	{ _frameQueue.back().mark(); }

	private:
		std::queue<SLeapFrame>			_frameQueue;
		CThreadMutex					_mutex;

		static SFrameQueue *			_instance;
};


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

class CLeapMotion
{
	public:
		CLeapMotion();
		~CLeapMotion();

		// Singleton methods.
		static CLeapMotion &			get()		{ return *_instance; }
		static void						create()	{ _instance = new CLeapMotion; }
		static void						destroy()	{ delete _instance; }

	private:
		Leap::Controller				_controller;
		class CLeapMotionListener *		_pListener;

		static CLeapMotion *			_instance;
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

	private:
		CLeapMotion *					_pLeap;
};


#endif // __IN_LEAP_H__