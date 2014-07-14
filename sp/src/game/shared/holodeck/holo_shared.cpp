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

EFinger	holo::LeapToHoloFingerCode( const Leap::Finger::Type &finger )
{
	switch( finger )
	{
		case Leap::Finger::TYPE_THUMB:
			return FINGER_THUMB;

		case Leap::Finger::TYPE_INDEX:
			return FINGER_POINTER;

		case Leap::Finger::TYPE_MIDDLE:
			return FINGER_MIDDLE;

		case Leap::Finger::TYPE_RING:
			return FINGER_USELESS;

		case Leap::Finger::TYPE_PINKY:
			return FINGER_PINKY;

		default:
			break;
	}

	Assert( 0 );
	return FINGER_COUNT;
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
		EFinger idx = LeapToHoloFingerCode( f.type() );
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

//=============================================================================
// SBallGesture implementation.
//=============================================================================
SBallGesture::SBallGesture()
{
	handId = INVALID_INDEX;
	radius = 0.0f;
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
	center = LeapToHoloCoordinates( h.sphereCenter() );
}
#endif

istream &holo::operator>>( istream &ss, SBallGesture &b )
{
	ss >> b.handId >> b.radius >> b.center;
	return ss;
}