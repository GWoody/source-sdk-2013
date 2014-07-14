/*
===============================================================================

	holo_shared.cpp
		Code\structures that are to be shared between the client and server.

===============================================================================
*/

#include "cbase.h"
#include "holo_shared.h"

using namespace std;
using namespace holo;

//-----------------------------------------------------------------------------
// Reads a Source vector from a stream.
//-----------------------------------------------------------------------------
static inline istream &operator>>( istream &ss, Vector &v )
{
	ss >> v.x >> v.y >> v.z;
	return ss;
}

//-----------------------------------------------------------------------------
// Writes a Source vector to a stream.
//-----------------------------------------------------------------------------
static inline ostream &operator<<( ostream &ss, const Vector &v )
{
	ss << v.x << " "
		<< v.y << " "
		<< v.z;
	return ss;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const char *EFinger::ToString( EFinger::type finger )
{
	switch( finger )
	{
		case FINGER_THUMB:
			return "FINGER_THUMB";

		case FINGER_POINTER:
			return "FINGER_POINTER";

		case FINGER_MIDDLE:
			return "FINGER_MIDDLE";

		case FINGER_RING:
			return "FINGER_RING";

		case FINGER_PINKY:
			return "FINGER_PINKY";

		case FINGER_COUNT:
			return "FINGER_COUNT";

		default:
			break;
	}

	return "FINGER_INVALID";
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const char *EGesture::ToString( EGesture::type gesture )
{
	switch( gesture )
	{
		case GESTURE_CIRCLE:
			return "GESTURE_CIRCLE";

		case GESTURE_SWIPE:
			return "GESTURE_SWIPE";

		case GESTURE_TAP:
			return "GESTURE_TAP";

		case GESTURE_COUNT:
			return "GESTURE_COUNT";

		default:
			break;
	}

	return "GESTURE_INVALID";
}

#ifdef CLIENT_DLL

//----------------------------------------------------------------------------
// Contains the code necessary to convert a Leap Motion Vector into Valve Vector.
//----------------------------------------------------------------------------
Vector holo::LeapToHoloCoordinates( const Leap::Vector &v )
{
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
	return ov;
}

EFinger::type holo::LeapToHoloFingerCode( const Leap::Finger::Type &finger )
{
	switch( finger )
	{
		case Leap::Finger::TYPE_THUMB:
			return EFinger::FINGER_THUMB;

		case Leap::Finger::TYPE_INDEX:
			return EFinger::FINGER_POINTER;

		case Leap::Finger::TYPE_MIDDLE:
			return EFinger::FINGER_MIDDLE;

		case Leap::Finger::TYPE_RING:
			return EFinger::FINGER_RING;

		case Leap::Finger::TYPE_PINKY:
			return EFinger::FINGER_PINKY;

		default:
			break;
	}

	Assert( 0 );
	return EFinger::FINGER_COUNT;
}

#endif

//=============================================================================
// SFinger implementation.
//=============================================================================
SFinger::SFinger()
{
	id = -1;
	width = length = 0.0f;
	direction = tipPosition = tipVelocity = vec3_origin;
}

#ifdef CLIENT_DLL
SFinger::SFinger( const Leap::Finger &f )
{
	FromLeap( f );
}

void SFinger::FromLeap( const Leap::Finger &f )
{
	id = f.id();
	direction = LeapToHoloCoordinates( f.direction() );
	tipPosition = LeapToHoloCoordinates( f.tipPosition() );
	tipVelocity = LeapToHoloCoordinates( f.tipVelocity() );
	width = f.width();
	length = f.length();
}
#endif

istream &holo::operator>>( istream &ss, SFinger &f )
{
	ss >> f.id >> f.direction >> f.tipPosition >> f.tipVelocity >> f.width >> f.length;
	return ss;
}

ostream &holo::operator<<( ostream &ss, const SFinger &f )
{
	ss << " " << f.id << " " << f.direction << " " << f.tipPosition << " " << f.tipVelocity << " " << f.width << " " << f.length;
	return ss;
}

//=============================================================================
// SHand implementation.
//=============================================================================
SHand::SHand()
{
	id = INVALID_INDEX;
	confidence = 0.0f;
	palmPosition = palmVelocity = palmNormal = vec3_origin;
}

#ifdef CLIENT_DLL
SHand::SHand( const Leap::Hand &h )
{
	FromLeap( h );
}

void SHand::FromLeap( const Leap::Hand &h )
{
	// Read all fingers.
	const Leap::FingerList &fingerlist = h.fingers();
	for each( const Leap::Finger &f in fingerlist )
	{
		EFinger::type idx = LeapToHoloFingerCode( f.type() );
		fingers[idx].FromLeap( f );
	}

	// Read hand.
	id = h.id();
	confidence = h.confidence();
	palmPosition = LeapToHoloCoordinates( h.palmPosition() );
	palmVelocity = LeapToHoloCoordinates( h.palmVelocity() );
	palmNormal = LeapToHoloCoordinates( h.palmNormal() );
}
#endif

istream &holo::operator>>( istream &ss, SHand &h )
{
	ss >> h.id >> h.confidence >> h.palmPosition >> h.palmVelocity >> h.palmNormal;

	for( int i = 0; i < EFinger::FINGER_COUNT; i++ )
	{
		ss >> h.fingers[i];
	}

	return ss;
}

ostream &holo::operator<<( ostream &ss, const SHand &h )
{
	ss << " " << h.id << " " << h.confidence << " " << h.palmPosition << " " << h.palmVelocity << " " << h.palmNormal;

	for( int i = 0; i < EFinger::FINGER_COUNT; i++ )
	{
		ss << h.fingers[i];
	}

	return ss;
}

//=============================================================================
// SCircleGesture implementation.
//=============================================================================
SCircleGesture::SCircleGesture()
{
	handId = fingerId = INVALID_INDEX;
	radius = 0.0f;
	center = normal = vec3_origin;
}

#ifdef CLIENT_DLL
SCircleGesture::SCircleGesture( const Leap::CircleGesture &c )
{
	FromLeap( c );
}

void SCircleGesture::FromLeap( const Leap::CircleGesture &c )
{
	const Leap::HandList &hands = c.hands();

	handId = hands[0].id();
	fingerId = c.pointable().id();
	radius = c.radius();
	center = LeapToHoloCoordinates( c.center() );
	normal = LeapToHoloCoordinates( c.normal() );
}
#endif

istream &holo::operator>>( istream &ss, SCircleGesture &c )
{
	ss >> c.handId >> c.fingerId >> c.center >> c.normal >> c.radius;
	return ss;
}

ostream &holo::operator<<( ostream &ss, const SCircleGesture &c )
{
	ss << c.handId << " " << c.fingerId << " " << c.center << " " << c.normal << " " << c.radius;
	return ss;
}

//=============================================================================
// SSwipeGesture implementation.
//=============================================================================
SSwipeGesture::SSwipeGesture()
{
	handId = INVALID_INDEX;
	speed = 0;
	direction = curPosition = startPosition = vec3_origin;
}

#ifdef CLIENT_DLL
SSwipeGesture::SSwipeGesture( const Leap::SwipeGesture &s )
{
	FromLeap( s );
}

void SSwipeGesture::FromLeap( const Leap::SwipeGesture &s )
{
	const Leap::HandList &hands = s.hands();

	handId = hands[0].id();
	speed = s.speed();
	direction = LeapToHoloCoordinates( s.direction() );
	curPosition = LeapToHoloCoordinates( s.position() );
	startPosition = LeapToHoloCoordinates( s.startPosition() );
}
#endif

istream &holo::operator>>( istream &ss, SSwipeGesture &s )
{
	ss >> s.handId >> s.speed >> s.direction >> s.startPosition >> s.curPosition;
	return ss;
}

ostream &holo::operator<<( ostream &ss, const SSwipeGesture &s )
{
	ss << s.handId << " " << s.speed << " " << s.direction << " " << s.startPosition << " " << s.curPosition;
	return ss;
}

//=============================================================================
// STapGesture implementation.
//=============================================================================
STapGesture::STapGesture()
{
	handId = fingerId = INVALID_INDEX;
	direction = position = vec3_origin;
}

#ifdef CLIENT_DLL
STapGesture::STapGesture( const Leap::KeyTapGesture &k )
{
	FromLeap( k );
}

STapGesture::STapGesture( const Leap::ScreenTapGesture &s )
{
	FromLeap( s );
}

void STapGesture::FromLeap( const Leap::KeyTapGesture &k )
{
	const Leap::HandList &hands = k.hands();

	handId = hands[0].id();
	fingerId = k.pointable().id();
	direction = LeapToHoloCoordinates( k.direction() );
	position = LeapToHoloCoordinates( k.position() );
}

void STapGesture::FromLeap( const Leap::ScreenTapGesture &s )
{
	const Leap::HandList &hands = s.hands();

	handId = hands[0].id();
	fingerId = s.pointable().id();
	direction = LeapToHoloCoordinates( s.direction() );
	position = LeapToHoloCoordinates( s.position() );
}
#endif

istream &holo::operator>>( istream &ss, STapGesture &t )
{
	ss >> t.handId >> t.fingerId >> t.direction >> t.position;
	return ss;
}

ostream &holo::operator<<( ostream &ss, const STapGesture &t )
{
	ss << t.handId << " " << t.fingerId << " " << t.direction << " " << t.position;
	return ss;
}

//=============================================================================
// SBallGesture implementation.
//=============================================================================
SBallGesture::SBallGesture()
{
	handId = INVALID_INDEX;
	radius = grabStrength = 0.0f;
	center = vec3_origin;
}

#ifdef CLIENT_DLL
SBallGesture::SBallGesture( const Leap::Hand &h )
{
	FromLeap( h );
}

void SBallGesture::FromLeap( const Leap::Hand &h )
{
	handId = h.id();
	radius = h.sphereRadius();
	grabStrength = h.grabStrength();
	center = LeapToHoloCoordinates( h.sphereCenter() );
}
#endif

istream &holo::operator>>( istream &ss, SBallGesture &b )
{
	ss >> b.handId >> b.radius >> b.grabStrength >> b.center;
	return ss;
}

ostream &holo::operator<<( ostream &ss, const SBallGesture &b )
{
	ss << b.handId << " " << b.radius << " " << b.grabStrength << " " << b.center;
	return ss;
}

//=============================================================================
// SFrame implementation.
//=============================================================================
SFrame::SFrame()
{
	_gestureBits = 0;
	_marked = false;
}

#ifdef CLIENT_DLL
SFrame::SFrame( const Leap::Frame &f )
{
	FromLeap( f );
}

void SFrame::FromLeap( const Leap::Frame &f )
{
	const Leap::GestureList &gestures = f.gestures();
	const Leap::GestureList::const_iterator &end = gestures.end();
	const Leap::HandList &hands = f.hands();

	_gestureBits = 0;

	for (Leap::GestureList::const_iterator it = gestures.begin(); it != end; it++)
	{
		const Leap::Gesture &gesture = *it;
		string data;
		if ( gesture.type() == Leap::Gesture::TYPE_CIRCLE )
		{
			_circle = SCircleGesture( gesture );
			SetGestureActive( EGesture::GESTURE_CIRCLE );
		}
		else if ( gesture.type() == Leap::Gesture::TYPE_SWIPE )
		{
			_swipe = SSwipeGesture( gesture );
			SetGestureActive( EGesture::GESTURE_SWIPE );
		}
		else if ( gesture.type() == Leap::Gesture::TYPE_KEY_TAP )
		{
			Leap::KeyTapGesture tap( gesture );
			_tap = STapGesture( tap );
			SetGestureActive( EGesture::GESTURE_TAP );
		}
		else if (gesture.type() == Leap::Gesture::TYPE_SCREEN_TAP )
		{
			Leap::ScreenTapGesture tap( gesture );
			_tap = STapGesture( tap );
			SetGestureActive( EGesture::GESTURE_TAP );
		}
	}

	if (!hands.isEmpty())
	{
		// Holodeck only needs to support a single hand for now.
#if 0
		for ( Leap::HandList::const_iterator it = hands.begin(); it != hands.end(); it++ )
		{
			const Leap::Hand &hand = *it;
#else
		{
			const Leap::Hand &hand = hands[0];
#endif
			_hand = SHand( hand );
			_ball = SBallGesture( hand );
		}
	}
}
#endif

istream &holo::operator>>( istream &ss, SFrame &f )
{
	ss >> f._hand >> f._gestureBits >> f._ball >> f._circle >> f._swipe >> f._tap;
	return ss;
}

ostream &holo::operator<<( ostream &ss, const SFrame &f )
{
	ss << " " << f._hand << " " << f._gestureBits << " " << f._ball << " " << f._circle << " " << f._swipe << " " << f._tap;
	return ss;
}