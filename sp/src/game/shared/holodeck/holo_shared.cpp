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

static ConVar holo_arm_length( "holo_arm_length", "650", FCVAR_ARCHIVE, "Users arm length in mm" );

//===============================================================================
// Leap -> Source conversion.
//===============================================================================
#ifdef CLIENT_DLL
	Vector			LeapToSourceVector( const Leap::Vector &v, bool transform = false );
	EFinger			LeapToSourceFingerCode( const Leap::Finger::Type &finger );
	float			LeapToSourceDistance( float distance );
#endif

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
const char *EFingerToString( EFinger finger )
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
const char *EGestureToString( EGesture gesture )
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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const char *EGlobalsToString( EGlobals global )
{
	switch( global )
	{
		case INVALID_INDEX:
			return "INVALID_INDEX";

		default:
			break;
	}

	return "EGLOBAL_INVALID_CODE";
}

#ifdef CLIENT_DLL

//----------------------------------------------------------------------------
// Contains the code necessary to convert a Leap Motion Vector into Valve Vector.
// 
// Set `transform` to `true` when dealing with position vectors, `false` for
// other vectors (normal, velocity, direction, etc).
//----------------------------------------------------------------------------
Vector LeapToSourceVector( const Leap::Vector &v, bool transform /*= false*/ )
{
	Leap::Vector transformed = v;
	Vector ov;

	if( transform )
	{
		// Set the origin of the Leap space to be 20cm above, and 50cm behind the device.
		transformed += Leap::Vector( 0, -200, -holo_arm_length.GetInt() );
	}

	// Source uses	{ forward, left, up }.
	// Leap uses	{ right, up, back }.
	ov.x = -transformed.z;
	ov.y = -transformed.x;
	ov.z = transformed.y;

	ov.x = LeapToSourceDistance( ov.x );
	ov.y = LeapToSourceDistance( ov.y );
	ov.z = LeapToSourceDistance( ov.z );
	return ov;
}

float LeapToSourceDistance( float distance )
{
	// Leap uses millimeters, Source uses inches.
	// The player is 72 units tall, which is estimated to be [5ft 10in]\[1.778m].
	// This gives us a meters to Source unit factor of:
	static const float scaleFactor = 1.778f / 72.0f;	// meters / unit
	return distance * scaleFactor;
}

EFinger LeapToSourceFingerCode( const Leap::Finger::Type &finger )
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
// SBone implementation.
//=============================================================================

SBone::SBone()
{
	nextJoint = prevJoint = vec3_origin;
}

#ifdef CLIENT_DLL

SBone::SBone(const Leap::Bone &b)
{
	FromLeap( b );
}

void SBone::FromLeap(const Leap::Bone &b)
{
	nextJoint = LeapToSourceVector( b.nextJoint(), true );
	prevJoint = LeapToSourceVector( b.prevJoint(), true );
}

#endif

void SBone::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteBitVec3Coord( nextJoint );
	buf->WriteBitVec3Coord( prevJoint );
}

void SBone::FromBitBuffer( bf_read *buf )
{
	buf->ReadBitVec3Coord( nextJoint );
	buf->ReadBitVec3Coord( prevJoint );
}

std::istream &operator>>(std::istream &ss, SBone &b)
{
	ss >> b.nextJoint >> b.prevJoint;

	return ss;
}

std::ostream &operator<<(std::ostream &ss, const SBone &b)
{
	ss << b.nextJoint << " " << b.prevJoint;

	return ss;
}

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
	direction = LeapToSourceVector( f.direction() );
	tipPosition = LeapToSourceVector( f.tipPosition(), true );
	tipVelocity = LeapToSourceVector( f.tipVelocity() );
	width = LeapToSourceDistance( f.width() );
	length = LeapToSourceDistance( f.length() );

	direction.NormalizeInPlace();
}
#endif

void SFinger::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteVarInt32( id );
	buf->WriteBitVec3Normal( direction );
	buf->WriteBitVec3Coord( tipPosition );
	buf->WriteBitVec3Normal( tipVelocity );
	buf->WriteFloat( width );
	buf->WriteFloat( length );
}

void SFinger::FromBitBuffer( bf_read *buf )
{
	id = buf->ReadVarInt32();
	buf->ReadBitVec3Normal( direction );
	buf->ReadBitVec3Coord( tipPosition );
	buf->ReadBitVec3Normal( tipVelocity );
	width = buf->ReadFloat();
	length = buf->ReadFloat();
}

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
	direction = position = velocity = normal = vec3_origin;
}

#ifdef CLIENT_DLL
SHand::SHand( const Leap::Hand &h )
{
	FromLeap( h );
}

void SHand::FromLeap( const Leap::Hand &h )
{
	BuildFingers( h );

	id = h.id();
	confidence = h.confidence();
	velocity = LeapToSourceVector( h.palmVelocity() );
	normal = LeapToSourceVector( h.palmNormal() );
	direction = LeapToSourceVector( h.direction() );
	position = LeapToSourceVector( h.palmPosition(), true );

	normal.NormalizeInPlace();
	direction.NormalizeInPlace();
}

void SHand::BuildFingers( const Leap::Hand &h )
{
	const Leap::FingerList &fingerlist = h.fingers();
	for each( const Leap::Finger &f in fingerlist )
	{
		EFinger idx = LeapToSourceFingerCode( f.type() );
		fingers[idx].FromLeap( f );
	}
}
#endif

void SHand::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteVarInt32( id );
	buf->WriteFloat( confidence );
	buf->WriteBitVec3Normal( direction );
	buf->WriteBitVec3Normal( normal );
	buf->WriteBitVec3Coord( position );
	buf->WriteBitVec3Normal( velocity );

	for( int i = 0; i < EFinger::FINGER_COUNT; i++ )
	{
		fingers[i].ToBitBuffer( buf );
	}
}

void SHand::FromBitBuffer( bf_read *buf )
{
	id = buf->ReadVarInt32();
	confidence = buf->ReadFloat();
	buf->ReadBitVec3Normal( direction );
	buf->ReadBitVec3Normal( normal );
	buf->ReadBitVec3Coord( position );
	buf->ReadBitVec3Normal( velocity );

	for( int i = 0; i < EFinger::FINGER_COUNT; i++ )
	{
		fingers[i].FromBitBuffer( buf );
	}
}

istream &holo::operator>>( istream &ss, SHand &h )
{
	ss >> h.id >> h.confidence >> h.position >> h.velocity >> h.normal >> h.direction;

	for( int i = 0; i < EFinger::FINGER_COUNT; i++ )
	{
		ss >> h.fingers[i];
	}

	return ss;
}

ostream &holo::operator<<( ostream &ss, const SHand &h )
{
	ss << " " << h.id << " " << h.confidence << " " << h.position << " " << h.velocity << " " << h.normal << " " << h.direction;

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
	radius = LeapToSourceDistance( c.radius() );
	center = LeapToSourceVector( c.center(), true );
	normal = LeapToSourceVector( c.normal() );
	duration = c.durationSeconds();

	// According to the Leap SDK:
	// If you draw the circle clockwise, the normal vector points in the same general direction as the pointable object drawing the circle.
	// If you draw the circle counterclockwise, the normal points back toward the pointable.
	//
	// Therefore if the angle to the normal (from the pointable) is less than 90 degrees (PI/2 radians) the directions are roughly the same,
	// which means the circle is clockwise.
	const Leap::Vector &pointableDir = c.pointable().direction();
	clockwise = pointableDir.angleTo( c.normal() ) <= ( M_PI / 2 );
}
#endif

void SCircleGesture::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteVarInt32( handId );
	buf->WriteVarInt32( fingerId );
	buf->WriteFloat( radius );
	buf->WriteFloat( duration );
	buf->WriteBitVec3Coord( center );
	buf->WriteBitVec3Normal( normal );
	buf->WriteVarInt32( clockwise );
}

void SCircleGesture::FromBitBuffer( bf_read *buf )
{
	handId = buf->ReadVarInt32();
	fingerId = buf->ReadVarInt32();
	radius = buf->ReadFloat();
	duration = buf->ReadFloat();
	buf->ReadBitVec3Coord( center );
	buf->ReadBitVec3Normal( normal );
	clockwise = buf->ReadVarInt32() != 0 ? true : false;
}

istream &holo::operator>>( istream &ss, SCircleGesture &c )
{
	ss >> c.handId >> c.fingerId >> c.center >> c.normal >> c.radius >> c.duration >> c.clockwise;
	return ss;
}

ostream &holo::operator<<( ostream &ss, const SCircleGesture &c )
{
	ss << c.handId << " " << c.fingerId << " " << c.center << " " << c.normal << " " << c.radius << " " << c.duration << " " << c.clockwise;
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
	direction = LeapToSourceVector( s.direction() );
	curPosition = LeapToSourceVector( s.position(), true );
	startPosition = LeapToSourceVector( s.startPosition(), true );
}
#endif

void SSwipeGesture::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteVarInt32( handId );
	buf->WriteFloat( speed );
	buf->WriteBitVec3Normal( direction );
	buf->WriteBitVec3Coord( curPosition );
	buf->WriteBitVec3Coord( startPosition );
}

void SSwipeGesture::FromBitBuffer( bf_read *buf )
{
	handId = buf->ReadVarInt32();
	speed = buf->ReadFloat();
	buf->ReadBitVec3Normal( direction );
	buf->ReadBitVec3Coord( curPosition );
	buf->ReadBitVec3Coord( startPosition );
}

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
	direction = LeapToSourceVector( k.direction() );
	position = LeapToSourceVector( k.position(), true );
}

void STapGesture::FromLeap( const Leap::ScreenTapGesture &s )
{
	const Leap::HandList &hands = s.hands();

	handId = hands[0].id();
	fingerId = s.pointable().id();
	direction = LeapToSourceVector( s.direction() );
	position = LeapToSourceVector( s.position(), true );
}
#endif

void STapGesture::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteVarInt32( handId );
	buf->WriteVarInt32( fingerId );
	buf->WriteBitVec3Normal( direction );
	buf->WriteBitVec3Coord( position );
}

void STapGesture::FromBitBuffer( bf_read *buf )
{
	handId = buf->ReadVarInt32();
	fingerId = buf->ReadVarInt32();
	buf->ReadBitVec3Normal( direction );
	buf->ReadBitVec3Coord( position );
}

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
	radius = LeapToSourceDistance( h.sphereRadius() );
	grabStrength = h.grabStrength();
	center = LeapToSourceVector( h.sphereCenter(), true );
}
#endif

void SBallGesture::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteSignedVarInt32( handId );
	buf->WriteFloat( radius );
	buf->WriteFloat( grabStrength );
	buf->WriteBitVec3Coord( center );
}

void SBallGesture::FromBitBuffer( bf_read *buf )
{
	handId = buf->ReadSignedVarInt32();
	radius = buf->ReadFloat();
	grabStrength = buf->ReadFloat();
	buf->ReadBitVec3Coord( center );
}

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
		_hand = SHand( hands[0] );
		_ball = SBallGesture( hands[0] );
	}
}
#endif

void SFrame::ToBitBuffer( bf_write *buf ) const
{
	_hand.ToBitBuffer( buf );
	_ball.ToBitBuffer( buf );
	buf->WriteVarInt32( _gestureBits );

	if( IsGestureActive( GESTURE_CIRCLE ) )
	{
		_circle.ToBitBuffer( buf );
	}

	if( IsGestureActive( GESTURE_SWIPE ) )
	{
		_swipe.ToBitBuffer( buf );
	}

	if( IsGestureActive( GESTURE_TAP ) )
	{
		_tap.ToBitBuffer( buf );
	}
}

void SFrame::FromBitBuffer( bf_read *buf )
{
	_hand.FromBitBuffer( buf );
	_ball.FromBitBuffer( buf );
	_gestureBits = buf->ReadVarInt32();

	if( IsGestureActive( GESTURE_CIRCLE ) )
	{
		_circle.FromBitBuffer( buf );
	}

	if( IsGestureActive( GESTURE_SWIPE ) )
	{
		_swipe.FromBitBuffer( buf );
	}

	if( IsGestureActive( GESTURE_TAP ) )
	{
		_tap.FromBitBuffer( buf );
	}
}

#ifdef GAME_DLL
// Transforms the frame data to be positioned relative to the given entity.
void SFrame::ToEntitySpace( CBaseCombatCharacter *entity, const Vector &delta )
{
	ApplyRotation( entity );

	Vector offset = entity->GetAbsOrigin() + delta;
	ApplyTranslation( offset );
}

void SFrame::ApplyTranslation( const Vector &offset )
{
	_ball.center += offset;
	_circle.center += offset;
	_hand.position += offset;
	_swipe.curPosition += offset;
	_swipe.startPosition += offset;
	_tap.position += offset;

	for( int i = 0; i < FINGER_COUNT; i++ )
	{
		_hand.fingers[i].tipPosition += offset;
	}
}

void SFrame::ApplyRotation( CBaseCombatCharacter *entity )
{
	// Convert the players direction vector to angles.
	QAngle ownerAngles;
	VectorAngles( entity->BodyDirection2D(), ownerAngles );

	const float yawAngle = ownerAngles.y;

	// We only need to rotate the forward and side components of the vector.
	// Height can be left alone.
	VectorYawRotate( _ball.center, yawAngle, _ball.center );
	VectorYawRotate( _circle.center, yawAngle, _circle.center );
	VectorYawRotate( _circle.normal, yawAngle, _circle.normal );
	VectorYawRotate( _hand.position, yawAngle, _hand.position );
	VectorYawRotate( _hand.normal, yawAngle, _hand.normal );
	VectorYawRotate( _hand.direction, yawAngle, _hand.direction );
	VectorYawRotate( _swipe.direction, yawAngle, _swipe.direction );
	VectorYawRotate( _swipe.curPosition, yawAngle, _swipe.curPosition );
	VectorYawRotate( _swipe.startPosition, yawAngle, _swipe.startPosition );
	VectorYawRotate( _tap.position, yawAngle, _tap.position );
	VectorYawRotate( _tap.direction, yawAngle, _tap.direction );

	for( int i = 0; i < FINGER_COUNT; i++ )
	{
		VectorYawRotate( _hand.fingers[i].tipPosition, yawAngle, _hand.fingers[i].tipPosition );
		VectorYawRotate( _hand.fingers[i].direction, yawAngle, _hand.fingers[i].direction );
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