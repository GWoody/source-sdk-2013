#ifndef __IN_LEAP_H__
#define __IN_LEAP_H__

#include <tier0/threadtools.h>

#include <Leap.h> 
#include <queue>
#include <string>
#include <sstream>

#include "holodeck/holo_shared.h"

//----------------------------------------------------------------------------
// This is thread safe.
//----------------------------------------------------------------------------
class SFrameQueue
{
	public:
		void							pushOnToQueue( const holo::SFrame &frame );
		holo::SFrame					popOffQueue();
		holo::SFrame 					peek();
		bool							isEmpty();

		void							markLast()	{ _frameQueue.back().Mark(); }

	private:
		std::queue<holo::SFrame>		_frameQueue;
		CThreadMutex					_mutex;
};


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class CLeapMotion
{
	public:
		// Singleton methods.
		static CLeapMotion &			get()		{ return *_instance; }
		static void						create()	{ _instance = new CLeapMotion; }
		static void						destroy()	{ delete _instance; }

		// Accessors.
		SFrameQueue &					getQueue()	{ return *_queue; }

	private:
		CLeapMotion();
		~CLeapMotion();

		Leap::Controller				_controller;
		class CLeapMotionListener *		_pListener;
		SFrameQueue *					_queue;

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

	private:
		CLeapMotion *					_pLeap;
};


#endif // __IN_LEAP_H__