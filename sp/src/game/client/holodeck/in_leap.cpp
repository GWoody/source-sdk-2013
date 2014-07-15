/*
==============================================================================

	in_leap.cpp
	Implements the Frame Queue and Leap Interface. 

==============================================================================
*/

#include "cbase.h"
#include "in_leap.h"
#include "holodeck/holo_shared.h"

using namespace std;
using namespace holo;

CLeapMotion *CLeapMotion::_instance;

//----------------------------------------------------------------------------
// Contains the code necessary to push a string onto an STL defined queue.
//----------------------------------------------------------------------------
void SFrameQueue::pushOnToQueue( const SFrame &frame )
{
	_mutex.Lock();
		// Disregard frames older than 1 second (Leap Motion samples at 60 frames per second).
		if( _frameQueue.size() >= 60 )
		{
			_frameQueue.pop();
		}

		_frameQueue.push( frame );
	_mutex.Unlock();
}

//----------------------------------------------------------------------------
// Contains the code necessary to pop off a string in a STL defined queue.
//----------------------------------------------------------------------------
SFrame SFrameQueue::popOffQueue()
{
	SFrame frame;

	_mutex.Lock();
		if( !_frameQueue.empty() )
		{
			frame = _frameQueue.front();
			_frameQueue.pop();
		}
	_mutex.Unlock();

	return frame;
}

//----------------------------------------------------------------------------
// Provides a preview of the top frame in the queue.
//----------------------------------------------------------------------------
SFrame SFrameQueue::peek()
{
	SFrame frame;

	_mutex.Lock();
		if( !_frameQueue.empty() )
		{
			frame = _frameQueue.front();
		}
	_mutex.Unlock();

	return frame;
}

//----------------------------------------------------------------------------
// Contains the code to check whether the queue is empty.
//----------------------------------------------------------------------------
bool SFrameQueue::isEmpty()
{
	_mutex.Lock();
		bool empty = _frameQueue.empty();
	_mutex.Unlock();

	return empty;
}

//----------------------------------------------------------------------------
// Contains the code to create a new Listener and attach it to a controller.
//----------------------------------------------------------------------------
CLeapMotion::CLeapMotion()
{
	_queue = new SFrameQueue;

	_pListener = new CLeapMotionListener( this );
	_controller.addListener( *_pListener );
}

//----------------------------------------------------------------------------
// Contains the code to remove the listener from the controller and destroy it. 
//----------------------------------------------------------------------------
CLeapMotion::~CLeapMotion()
{
	_controller.removeListener( *_pListener );
	delete _pListener;

	delete _queue;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CLeapMotion::CreateMove( CUserCmd *cmd )
{
	cmd->holo_frame = BuildFinalFrame();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
SFrame CLeapMotion::BuildFinalFrame()
{
	SFrame finalFrame;

	if( !_queue->isEmpty() )
	{
		// Mark the position of the (current) last frame.
		_queue->markLast();

		holo::SFrame curframe;

		do
		{
			curframe = _queue->popOffQueue();

			if( curframe.IsGestureActive( EGesture::GESTURE_CIRCLE ) )
			{
				finalFrame._circle = curframe._circle;
				finalFrame.SetGestureActive( EGesture::GESTURE_CIRCLE );
			}
			if( curframe.IsGestureActive( EGesture::GESTURE_SWIPE ) )
			{
				finalFrame._swipe = curframe._swipe;
				finalFrame.SetGestureActive( EGesture::GESTURE_SWIPE );
			}
			if( curframe.IsGestureActive( EGesture::GESTURE_TAP ) )
			{
				finalFrame._tap = curframe._tap;
				finalFrame.SetGestureActive( EGesture::GESTURE_TAP );
			}

			finalFrame._ball = curframe._ball;
			finalFrame._hand = curframe._hand;

		} while( !curframe.IsMarked() );
	}

	//Warning( "%f %f %f\n", finalFrame._hand.palmPosition.x, finalFrame._hand.palmPosition.y, finalFrame._hand.palmPosition.z );

	return finalFrame;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CLeapMotionListener::CLeapMotionListener(CLeapMotion *pLeap)
{

}

//----------------------------------------------------------------------------
// Contains the code to enable gesture usage in the application.
//----------------------------------------------------------------------------
void CLeapMotionListener::onConnect( const Leap::Controller &controller )
{
	controller.enableGesture( Leap::Gesture::TYPE_CIRCLE );
	controller.enableGesture( Leap::Gesture::TYPE_KEY_TAP );
	controller.enableGesture( Leap::Gesture::TYPE_SCREEN_TAP );
	controller.enableGesture( Leap::Gesture::TYPE_SWIPE );

	Warning( "Leap Motion connected!\n" );
}

//----------------------------------------------------------------------------
// Contains the code necessary to gather frame data and pass gesture information 
// into the proper utility functions. It is called repeatedly on each new Frame.
//----------------------------------------------------------------------------
void CLeapMotionListener::onFrame( const Leap::Controller &controller )
{
	const Leap::Frame &frame = controller.frame();
	SFrame newFrame( frame );

	// Add the constructed frame to the queue.
	CLeapMotion::get().getQueue().pushOnToQueue( newFrame );
}