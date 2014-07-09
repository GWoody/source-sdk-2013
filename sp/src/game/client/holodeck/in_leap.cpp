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

/*
==============================================================================
[pushOnToQueue]
Contains the code necessary to push a string onto an STL defined queue.
==============================================================================
*/


void SFrameQueue::pushOnToQueue( std::string s )
{
	_mutex.Lock();
	_frameQueue.push(s);
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

std::string SFrameQueue::popOffQueue()
{
	std::string topFrame = " ";
	_mutex.Lock();
	topFrame = _frameQueue.front();
	_frameQueue.pop();
	_mutex.Unlock();

	return topFrame;
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

CLeapMotion *gpLeapMotion = NULL;

void Leap_Create()
{
	gpLeapMotion = new CLeapMotion;
}

void Leap_Destroy()
{
	delete gpLeapMotion;
}

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
[LeapVectorToString]
Contains the code necessary to convert a Leap Motion defined vector into a protocol ready format.
==============================================================================
*/
std::string CLeapMotionListener::LeapVectorToString(Leap::Vector &vector)
{
	std::stringstream ss;

	ss << "vector " << vector.x << " " << vector.y << " " << vector.z << " " << "\n";

	std::string sVector = ss.str();

	return sVector;

}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/*
==============================================================================
[HeaderToString]
Contains the code necessary to convert a gesture into a protocol ready format.
==============================================================================
*/
std::string CLeapMotionListener::HeaderToString( const Leap::Frame & frame, CLeapMotion::typeTag_e enumeration )
{
	std::stringstream ss;

	uint32 tag = 0;

	switch (enumeration)
	{
	case CLeapMotion::L_VECTOR:
		tag = 0;
		break;
	case CLeapMotion::L_HAND:
		tag = 1;
		break;
	case CLeapMotion::L_FINGER:
		tag = 2;
		break;
	case CLeapMotion::L_CIRCLE:
		tag = 3;
		break;
	case CLeapMotion::L_SWIPE:
		tag = 4;
		break;
	case CLeapMotion::L_KTAP:
		tag = 5;
		break;
	case CLeapMotion::L_STAP:
		tag = 6;
		break;
	case CLeapMotion::L_BALL:
		tag = 7;
		break;
	default:
		tag = 99;
		Warning("error\n");
		break;
	}

	ss << "header " << tag << " " << frame.id() << " " << VERSION << "\n";

	std::string sHeader = ss.str();

	return sHeader;
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

	for (Leap::GestureList::const_iterator it = gestures.begin(); it != end; it++)
	{
		const Leap::Gesture &gesture = *it;
		if ( gesture.type() == Leap::Gesture::TYPE_CIRCLE )
		{
			Leap::CircleGesture circle = Leap::CircleGesture( gesture );
			std::string data = CircleGestureToString( circle );

		}
		if ( gesture.type() == Leap::Gesture::TYPE_SWIPE )
		{
			Leap::SwipeGesture swipe = Leap::SwipeGesture( gesture );
			std::string data = SwipeGestureToString( swipe );
		}
		if ( gesture.type() == Leap::Gesture::TYPE_KEY_TAP )
		{
			Leap::KeyTapGesture keyTap = Leap::KeyTapGesture( gesture );
			std::string data = KeyTapGestureToString( keyTap );
		}
		if (gesture.type() == Leap::Gesture::TYPE_SCREEN_TAP )
		{
			Leap::ScreenTapGesture screenTap = Leap::ScreenTapGesture( gesture );
			std::string data = ScreenTapGestureToString( screenTap );
		}

	}

	if (!hands.isEmpty())
	{
		for ( Leap::HandList::const_iterator it = hands.begin(); it != hands.end(); it++ )
		{
			const Leap::Hand &hand = *it;
			std::string data = HandToString( hand );

			if (hand.grabStrength() > 0.5)
			{
				std::string data = BallGestureToString(hand);
			}

		}
	}

	Leap::Vector newVector = Leap::Vector( 0.5, 200.3, 67 );
	std::string sVector = LeapVectorToString( newVector );

	CLeapMotion::typeTag_e test = CLeapMotion::L_SWIPE;

	std::string dummyHeader = HeaderToString(frame, test);
}


