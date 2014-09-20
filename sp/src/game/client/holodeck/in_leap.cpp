/*
==============================================================================

	in_leap.cpp
	Implements the Frame Queue and Leap Interface. 

==============================================================================
*/

#include "cbase.h"
#include "in_leap.h"
#include "in_buttons.h"
#include "holodeck/holo_shared.h"

using namespace std;
using namespace holo;

CLeapMotion *CLeapMotion::_instance;

//----------------------------------------------------------------------------
// Contains the code necessary to push a string onto an STL defined queue.
//----------------------------------------------------------------------------
void SFrameQueue::Push( const CFrame &frame )
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
CFrame SFrameQueue::Pop()
{
	CFrame frame;

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
CFrame SFrameQueue::Peek()
{
	CFrame frame;

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
bool SFrameQueue::IsEmpty()
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
	Warning("################## %d\n", sizeof(CFrame) );
	_queue = new SFrameQueue;

	_controller.addListener( _listener );
}

//----------------------------------------------------------------------------
// Contains the code to remove the listener from the controller and destroy it. 
//----------------------------------------------------------------------------
CLeapMotion::~CLeapMotion()
{
	_controller.removeListener( _listener );

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
CFrame CLeapMotion::BuildFinalFrame()
{
	CFrame finalFrame;

	bool empty = _queue->IsEmpty();
	finalFrame.SetValid( !empty );
	
	while( !_queue->IsEmpty() )
	{
		CFrame curframe = _queue->Pop();

		if( curframe.IsGestureActive( EGesture::GESTURE_CIRCLE ) )
		{
			finalFrame.SetCircleGesture( curframe.GetCircleGesture() );
		}
		if( curframe.IsGestureActive( EGesture::GESTURE_SWIPE ) )
		{
			finalFrame.SetSwipeGesture( curframe.GetSwipeGesture() );
		}
		if( curframe.IsGestureActive( EGesture::GESTURE_TAP ) )
		{
			finalFrame.SetTapGesture( curframe.GetTapGesture() );
		}

		finalFrame.SetHand( curframe.GetHand( EHand::LEFT ), EHand::LEFT );
		finalFrame.SetHand( curframe.GetHand( EHand::RIGHT ), EHand::RIGHT );
	}

	return finalFrame;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CLeapMotionListener::CLeapMotionListener()
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
	CFrame newFrame( frame );

	// Add the constructed frame to the queue.
	CLeapMotion::Get().GetQueue().Push( newFrame );
}