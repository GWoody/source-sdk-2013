/*
==============================================================================

	in_leap.cpp
	Implements the Frame Queue and Leap Interface. 

==============================================================================
*/

#include "cbase.h"
#include "in_leap.h"

static const int VERSION = 1;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
SLeapFrame::SLeapFrame( float time )
{
	gametime = time;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool SLeapFrame::isEmpty()
{
	return data.empty();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SLeapFrame::push( const std::string &str )
{
	data.push( str );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
std::string SLeapFrame::pop()
{
	std::string str = "";

	if( !data.empty() )
	{
		str = data.front();
		data.pop();
	}

	return str;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

SFrameQueue *SFrameQueue::_instance;

/*
==============================================================================
[pushOnToQueue]
Contains the code necessary to push a string onto an STL defined queue.
==============================================================================
*/


void SFrameQueue::pushOnToQueue( const SLeapFrame &frame )
{
	_mutex.Lock();
		_frameQueue.push( frame );
	_mutex.Unlock();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/*
==============================================================================
[pushOffQueue]
Contains the code necessary to pop off a string in a STL defined queue.
==============================================================================
*/

SLeapFrame SFrameQueue::popOffQueue()
{
	SLeapFrame frame( 0.0f );

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
//----------------------------------------------------------------------------

/*
==============================================================================
[peek]
Provides a preview of the top frame in the queue.
==============================================================================
*/

 SLeapFrame SFrameQueue::peek()
{
	SLeapFrame frame( 0.0f );

	_mutex.Lock();
		if( !_frameQueue.empty() )
		{
			frame = _frameQueue.front();
		}
	_mutex.Unlock();

	return frame;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/*
==============================================================================
[isEmpty]
Contains the code to check whether the queue is empty.
==============================================================================
*/

bool SFrameQueue::isEmpty()
{
	if (!_frameQueue.empty())
	{
		return true;
	}
	else
		return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

CLeapMotion *CLeapMotion::_instance;

/*
==============================================================================
[CLeapMotion]
Contains the code to create a new Listener and attach it to a controller.
==============================================================================
*/

CLeapMotion::CLeapMotion()
{
	_pListener = new CLeapMotionListener( this );
	_controller.addListener( *_pListener );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/*
==============================================================================
[~CleapMotion]
Contains the code to remove the listener from the controller and destroy it. 
==============================================================================
*/

CLeapMotion::~CLeapMotion()
{
	_controller.removeListener( *_pListener );
	delete _pListener;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CLeapMotion::setEngineTime( float curtime )
{
	_timerMutex.Lock();
		_engineTime = curtime;
	_timerMutex.Unlock();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float CLeapMotion::getEngineTime()
{
	_timerMutex.Lock();
		float curtime = _engineTime;
	_timerMutex.Unlock();

	return curtime;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

CLeapMotionListener::CLeapMotionListener(CLeapMotion *pLeap)
{

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/*
==============================================================================
[onConnect]
Contains the code to enable gesture usage in the application.
==============================================================================
*/

void CLeapMotionListener::onConnect( const Leap::Controller &controller )
{
	controller.enableGesture( Leap::Gesture::TYPE_CIRCLE );
	controller.enableGesture( Leap::Gesture::TYPE_KEY_TAP );
	controller.enableGesture( Leap::Gesture::TYPE_SCREEN_TAP );
	controller.enableGesture( Leap::Gesture::TYPE_SWIPE );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/*
==============================================================================
[CircleGestureToString]
Contains the code necessary to convert a circle gesture into a protocol ready format.
==============================================================================
*/

std::string CLeapMotionListener::CircleGestureToString( const Leap::CircleGesture &circleGesture )
{
	std::stringstream ss;

	Leap::HandList handsForGesture = circleGesture.hands();

	ss << "circlegesture " << handsForGesture[0].id() << " " << circleGesture.pointable() << " " << circleGesture.center() << " " << circleGesture.normal() << " " << circleGesture.radius() << " " << circleGesture.state() << "\n";

	std::string sCircleGesture = ss.str();

	return sCircleGesture;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/*
==============================================================================
[SwipeGestureToString]
Contains the code necessary to convert a swipe gesture into a protocol ready format.
==============================================================================
*/
std::string CLeapMotionListener::SwipeGestureToString( const Leap::SwipeGesture &swipeGesture )
{
	std::stringstream ss;

	Leap::HandList handsForGesture = swipeGesture.hands();

	ss << "swipegesture " << swipeGesture.direction() << " " << swipeGesture.position() << " " << handsForGesture[0].id() << " " << swipeGesture.speed() << " " <<  swipeGesture.startPosition() << "\n";

	std::string sSwipeGesture = ss.str();

	return sSwipeGesture;

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/*
==============================================================================
[FingerToString]
Contains the code necessary to convert a Leap Motion finger into a protocol ready format.
==============================================================================
*/
std::string CLeapMotionListener::FingerToString( const Leap::Finger &finger)
{
	std::stringstream ss;

	ss << "finger " << finger.direction() << " " << finger.tipPosition() << " " << finger.tipVelocity() << " " << finger.width() << " " << finger.length() << "\n";

	std::string sFinger = ss.str();

	return sFinger;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/*
==============================================================================
[HandToString]
Contains the code necessary to convert a Leap Motion hand into a protocol ready format.
==============================================================================
*/
std::string CLeapMotionListener::HandToString( const Leap::Hand &hand )
{

	std::stringstream ss;

	ss << "hand " << hand.confidence() << " ";

	for each (Leap::Finger hFinger in hand.fingers())
	{
		std::string finger = FingerToString(hFinger);
		ss << finger << " ";
	}

	ss << hand.palmPosition() << " " << hand.palmVelocity() << " " << hand.palmNormal() << "\n";

	std::string sHand = ss.str();

	return sHand;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/*
==============================================================================
[KeyTapGestureToString]
Contains the code to convert a Leap Motion KeyTap Gesture into a protocol ready format.
==============================================================================
*/

std::string	CLeapMotionListener::KeyTapGestureToString( const Leap::KeyTapGesture &keyTapGesture)
{

	std::stringstream ss;

	Leap::HandList handsForGesture = keyTapGesture.hands();

	ss << "keytapgesture " << keyTapGesture.direction() << " " << keyTapGesture.position() << " " << handsForGesture[0].id() << " " << keyTapGesture.pointable() << "\n";

	std::string sKeyTapGestureToString = ss.str();

	return sKeyTapGestureToString;

}



//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/*
==============================================================================
[ScreenTapGestureToString]
Contains the code to convert a Leap Motion ScreenTap Gesture into a protocol ready format.
==============================================================================
*/
std::string CLeapMotionListener::ScreenTapGestureToString( const Leap::ScreenTapGesture &ScreenTapGesture )
{

	std::stringstream ss;

	Leap::HandList handsForGesture = ScreenTapGesture.hands();

	ss << "screentapgesture " << ScreenTapGesture.direction() << " " << ScreenTapGesture.position() << " " << handsForGesture[0].id() << " " << ScreenTapGesture.pointable() << "\n";

	std::string sKeyTapGestureToString = ss.str();

	return sKeyTapGestureToString;

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/*
==============================================================================
[BallGestureToString]
Contains the code necessary to check whether the user's hand has performed a ball gesture.
If so, it converts it into a protocol ready format.
==============================================================================
*/

std::string	CLeapMotionListener::BallGestureToString( const Leap::Hand &hand )
{
	std::stringstream ss;

	ss << "ballgesture " << hand.id() << " " << hand.sphereCenter() << " " << hand.sphereRadius() << " " << hand.palmPosition() << "\n";

	std::string sBallGesture = ss.str();

	return sBallGesture;

}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/*
==============================================================================
====
[ToSourceVector]
Contains the code necessary to convert a Leap Motion Vector into Valve Vector.

====
==============================================================================
*/

Vector ToSourceVector(const Leap::Vector &v)
{
	return Vector(v.x, v.y, v.z);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/*
==============================================================================
[onFrame]
Contains the code necessary to gather frame data and pass gesture information into
the proper utility functions. It is called repeatedly on each new Frame.
==============================================================================
*/

void CLeapMotionListener::onFrame( const Leap::Controller &controller )
{
	const Leap::Frame &frame = controller.frame();
	const Leap::GestureList &gestures = frame.gestures();
	const Leap::GestureList::const_iterator &end = gestures.end();
	const Leap::HandList &hands = frame.hands();

	float engineTime = CLeapMotion::get().getEngineTime();
	SLeapFrame strFrame( engineTime );

	for (Leap::GestureList::const_iterator it = gestures.begin(); it != end; it++)
	{
		const Leap::Gesture &gesture = *it;
		std::string data;
		if ( gesture.type() == Leap::Gesture::TYPE_CIRCLE )
		{
			Leap::CircleGesture circle = Leap::CircleGesture( gesture );
			data = CircleGestureToString( circle );
		}
		else if ( gesture.type() == Leap::Gesture::TYPE_SWIPE )
		{
			Leap::SwipeGesture swipe = Leap::SwipeGesture( gesture );
			data = SwipeGestureToString( swipe );
		}
		else if ( gesture.type() == Leap::Gesture::TYPE_KEY_TAP )
		{
			Leap::KeyTapGesture keyTap = Leap::KeyTapGesture( gesture );
			data = KeyTapGestureToString( keyTap );
		}
		else if (gesture.type() == Leap::Gesture::TYPE_SCREEN_TAP )
		{
			Leap::ScreenTapGesture screenTap = Leap::ScreenTapGesture( gesture );
			data = ScreenTapGestureToString( screenTap );
		}

		strFrame.push( data );
	}

	if (!hands.isEmpty())
	{
		for ( Leap::HandList::const_iterator it = hands.begin(); it != hands.end(); it++ )
		{
			const Leap::Hand &hand = *it;
			std::string data = HandToString( hand );
			strFrame.push( data );

			if (hand.grabStrength() > 0.5)
			{
				data = BallGestureToString(hand);
				strFrame.push( data );
			}
		}
	}

	// Add the constructed frame to the queue.
	SFrameQueue::get().pushOnToQueue( strFrame );
}