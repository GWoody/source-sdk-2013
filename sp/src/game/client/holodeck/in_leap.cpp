/*
==============================================================================

	in_leap.cpp
	Implements the Frame Queue and Leap Interface. 

==============================================================================
*/

#include "cbase.h"
#include "in_leap.h"
#include "holodeck/holo_shared.h"

static const int VERSION = 1;

//#define OUTPUT_DEBUG_STRINGS

//----------------------------------------------------------------------------
// Contains the code necessary to convert a Leap Motion Vector into Valve Vector.
//----------------------------------------------------------------------------
std::string ToSourceCoordinates(const Leap::Vector &v)
{
	std::ostringstream ss;
	Vector ov;

	// Source uses	{ forward, left, up }.
	// Leap uses	{ left, up, forward }.
	ov.x = v.z;
	ov.y = v.x;
	ov.z = v.y;

	// Leap uses millimeters, Source uses inches.

	// The player is 72 units tall (which is estimated to be 5ft 10in -> 1.778m).
	// This gives us a Source unit to millimeters factor of:
	const float scaleFactor = 72.0f / 0.001778f;	// units / millimeter.

	ov *= scaleFactor;

	ss << ov.x << " " << ov.y << " " << ov.z;
	return ss.str();
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
	SLeapFrame frame;

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
	SLeapFrame frame;

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
	return _frameQueue.empty();
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

	Warning( "Leap Motion connected!\n" );
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

	ss	<< "circlegesture " 
		<< handsForGesture[0].id() << " " 
		<< circleGesture.pointable().id() << " " 
		<< ToSourceCoordinates( circleGesture.center() ) << " " 
		<< ToSourceCoordinates( circleGesture.normal() ) << " " 
		<< circleGesture.radius() << "\n";

	std::string sCircleGesture = ss.str();

#ifdef OUTPUT_DEBUG_STRINGS
	Msg( "CircleGestureToString: %s\n", sCircleGesture.c_str() );
#endif

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

	ss << "swipegesture " 
		<< ToSourceCoordinates( swipeGesture.direction() ) << " " 
		<< ToSourceCoordinates( swipeGesture.position() ) << " " 
		<< handsForGesture[0].id() << " " 
		<< swipeGesture.speed() << " " 
		<< ToSourceCoordinates( swipeGesture.startPosition() ) << "\n";

	std::string sSwipeGesture = ss.str();

#ifdef OUTPUT_DEBUG_STRINGS
	Msg( "SwipeGestureToString: %s\n", sSwipeGesture.c_str() );
#endif

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

	ss << finger.id() << " " 
		<< ToSourceCoordinates( finger.direction() ) << " " 
		<< ToSourceCoordinates( finger.tipPosition() ) << " " 
		<< ToSourceCoordinates( finger.tipVelocity() ) << " " 
		<< finger.width() << " " 
		<< finger.length() << "\n";

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

	ss << "hand " << hand.fingers().count() << " ";

	for each (Leap::Finger hFinger in hand.fingers())
	{
		std::string finger = FingerToString(hFinger);
		ss << finger << " ";
	}

	ss << hand.id() << " " 
		<< hand.confidence() << " " 
		<< ToSourceCoordinates( hand.palmPosition() ) << " " 
		<< ToSourceCoordinates( hand.palmVelocity() ) << " " 
		<< ToSourceCoordinates( hand.palmNormal() ) << "\n";

	std::string sHand = ss.str();

#ifdef OUTPUT_DEBUG_STRINGS
	Msg( "HandToString: %s\n", sHand.c_str() );
#endif

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

	ss << "keytapgesture " 
		<< ToSourceCoordinates( keyTapGesture.direction() ) << " " 
		<< ToSourceCoordinates( keyTapGesture.position() ) << " " 
		<< handsForGesture[0].id() << " " 
		<< keyTapGesture.pointable().id() << "\n";

	std::string sKeyTapGestureToString = ss.str();

#ifdef OUTPUT_DEBUG_STRINGS
	Msg( "KeyTapGestureToString: %s\n", sKeyTapGestureToString.c_str() );
#endif

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

	ss << "screentapgesture " 
		<< ToSourceCoordinates( ScreenTapGesture.direction() ) << " " 
		<< ToSourceCoordinates( ScreenTapGesture.position() ) << " " 
		<< handsForGesture[0].id() << " " 
		<< ScreenTapGesture.pointable().id() << "\n";

	std::string sKeyTapGestureToString = ss.str();

#ifdef OUTPUT_DEBUG_STRINGS
	Msg( "ScreenTapGestureToString: %s\n", sKeyTapGestureToString.c_str() );
#endif

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

	ss << "ballgesture " 
		<< hand.id() << " " 
		<< ToSourceCoordinates( hand.sphereCenter() ) << " " 
		<< hand.sphereRadius() << " " 
		<< ToSourceCoordinates( hand.palmPosition() ) << "\n";

	std::string sBallGesture = ss.str();

#ifdef OUTPUT_DEBUG_STRINGS
	Msg( "BallGestureToString: %s\n", sBallGesture.c_str() );
#endif

	return sBallGesture;
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

	SLeapFrame strFrame;

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