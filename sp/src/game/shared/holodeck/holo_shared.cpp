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
// CBone implementation.
//=============================================================================
CBone::CBone()
{
	nextJoint = prevJoint = vec3_origin;
}

#ifdef CLIENT_DLL

CBone::CBone(const Leap::Bone &b)
{
	FromLeap( b );
}

void CBone::FromLeap(const Leap::Bone &b)
{
	nextJoint = LeapToSourceVector( b.nextJoint(), true );
	prevJoint = LeapToSourceVector( b.prevJoint(), true );
}

#endif

void CBone::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteBitVec3Coord( nextJoint );
	buf->WriteBitVec3Coord( prevJoint );
}

void CBone::FromBitBuffer( bf_read *buf )
{
	buf->ReadBitVec3Coord( nextJoint );
	buf->ReadBitVec3Coord( prevJoint );
}

void CBone::Transform( float yaw, const Vector &translation )
{
	
}

//=============================================================================
// CFinger implementation.
//=============================================================================
CFinger::CFinger()
{
	id = -1;
	width = length = 0.0f;
	direction = tipPosition = tipVelocity = vec3_origin;
}

#ifdef CLIENT_DLL
CFinger::CFinger( const Leap::Finger &f )
{
	FromLeap( f );
}

void CFinger::FromLeap( const Leap::Finger &f )
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

void CFinger::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteVarInt32( id );
	buf->WriteBitVec3Normal( direction );
	buf->WriteBitVec3Coord( tipPosition );
	buf->WriteBitVec3Normal( tipVelocity );
	buf->WriteFloat( width );
	buf->WriteFloat( length );
}

void CFinger::FromBitBuffer( bf_read *buf )
{
	id = buf->ReadVarInt32();
	buf->ReadBitVec3Normal( direction );
	buf->ReadBitVec3Coord( tipPosition );
	buf->ReadBitVec3Normal( tipVelocity );
	width = buf->ReadFloat();
	length = buf->ReadFloat();
}

void CFinger::Transform( float yaw, const Vector &translation )
{
	// Apply rotations.
	VectorYawRotate( tipPosition, yaw, tipPosition );
	VectorYawRotate( direction, yaw, direction );

	// Apply translations.
	tipPosition += translation;
}

//=============================================================================
// CHand implementation.
//=============================================================================
CHand::CHand()
{
	id = INVALID_INDEX;
	confidence = 0.0f;
	direction = position = velocity = normal = vec3_origin;
}

#ifdef CLIENT_DLL
CHand::CHand( const Leap::Hand &h )
{
	FromLeap( h );
}

void CHand::FromLeap( const Leap::Hand &h )
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

void CHand::BuildFingers( const Leap::Hand &h )
{
	const Leap::FingerList &fingerlist = h.fingers();
	for each( const Leap::Finger &f in fingerlist )
	{
		EFinger idx = LeapToSourceFingerCode( f.type() );
		fingers[idx].FromLeap( f );
	}
}
#endif

void CHand::ToBitBuffer( bf_write *buf ) const
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

void CHand::FromBitBuffer( bf_read *buf )
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

void CHand::Transform( float yaw, const Vector &translation )
{
	// Apply rotations.
	VectorYawRotate( position, yaw, position );
	VectorYawRotate( normal, yaw, normal );
	VectorYawRotate( direction, yaw, direction );

	// Apply translations.
	position += translation;
	
	// Transform fingers.
	for( int i = 0; i < FINGER_COUNT; i++ )
	{
		fingers[i].Transform( yaw, translation );
	}
}

//=============================================================================
// CCircleGesture implementation.
//=============================================================================
CCircleGesture::CCircleGesture()
{
	handId = fingerId = INVALID_INDEX;
	radius = 0.0f;
	center = normal = vec3_origin;
}

#ifdef CLIENT_DLL
CCircleGesture::CCircleGesture( const Leap::CircleGesture &c )
{
	FromLeap( c );
}

void CCircleGesture::FromLeap( const Leap::CircleGesture &c )
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

void CCircleGesture::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteVarInt32( handId );
	buf->WriteVarInt32( fingerId );
	buf->WriteFloat( radius );
	buf->WriteFloat( duration );
	buf->WriteBitVec3Coord( center );
	buf->WriteBitVec3Normal( normal );
	buf->WriteVarInt32( clockwise );
}

void CCircleGesture::FromBitBuffer( bf_read *buf )
{
	handId = buf->ReadVarInt32();
	fingerId = buf->ReadVarInt32();
	radius = buf->ReadFloat();
	duration = buf->ReadFloat();
	buf->ReadBitVec3Coord( center );
	buf->ReadBitVec3Normal( normal );
	clockwise = buf->ReadVarInt32() != 0 ? true : false;
}

void CCircleGesture::Transform( float yaw, const Vector &translation )
{
	// Apply rotations.
	VectorYawRotate( center, yaw, center );
	VectorYawRotate( normal, yaw, normal );

	// Apply translation.
	center += translation;
}

//=============================================================================
// CSwipeGesture implementation.
//=============================================================================
CSwipeGesture::CSwipeGesture()
{
	handId = INVALID_INDEX;
	speed = 0;
	direction = curPosition = startPosition = vec3_origin;
}

#ifdef CLIENT_DLL
CSwipeGesture::CSwipeGesture( const Leap::SwipeGesture &s )
{
	FromLeap( s );
}

void CSwipeGesture::FromLeap( const Leap::SwipeGesture &s )
{
	const Leap::HandList &hands = s.hands();

	handId = hands[0].id();
	speed = s.speed();
	direction = LeapToSourceVector( s.direction() );
	curPosition = LeapToSourceVector( s.position(), true );
	startPosition = LeapToSourceVector( s.startPosition(), true );
}
#endif

void CSwipeGesture::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteVarInt32( handId );
	buf->WriteFloat( speed );
	buf->WriteBitVec3Normal( direction );
	buf->WriteBitVec3Coord( curPosition );
	buf->WriteBitVec3Coord( startPosition );
}

void CSwipeGesture::FromBitBuffer( bf_read *buf )
{
	handId = buf->ReadVarInt32();
	speed = buf->ReadFloat();
	buf->ReadBitVec3Normal( direction );
	buf->ReadBitVec3Coord( curPosition );
	buf->ReadBitVec3Coord( startPosition );
}

void CSwipeGesture::Transform( float yaw, const Vector &translation )
{
	// Apply rotations.
	VectorYawRotate( direction, yaw, direction );
	VectorYawRotate( curPosition, yaw, curPosition );
	VectorYawRotate( startPosition, yaw, startPosition );

	// Apply translations.
	curPosition += translation;
	startPosition += translation;
}

//=============================================================================
// CTapGesture implementation.
//=============================================================================
CTapGesture::CTapGesture()
{
	handId = fingerId = INVALID_INDEX;
	direction = position = vec3_origin;
}

#ifdef CLIENT_DLL
CTapGesture::CTapGesture( const Leap::KeyTapGesture &k )
{
	FromLeap( k );
}

CTapGesture::CTapGesture( const Leap::ScreenTapGesture &s )
{
	FromLeap( s );
}

void CTapGesture::FromLeap( const Leap::KeyTapGesture &k )
{
	const Leap::HandList &hands = k.hands();

	handId = hands[0].id();
	fingerId = k.pointable().id();
	direction = LeapToSourceVector( k.direction() );
	position = LeapToSourceVector( k.position(), true );
}

void CTapGesture::FromLeap( const Leap::ScreenTapGesture &s )
{
	const Leap::HandList &hands = s.hands();

	handId = hands[0].id();
	fingerId = s.pointable().id();
	direction = LeapToSourceVector( s.direction() );
	position = LeapToSourceVector( s.position(), true );
}
#endif

void CTapGesture::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteVarInt32( handId );
	buf->WriteVarInt32( fingerId );
	buf->WriteBitVec3Normal( direction );
	buf->WriteBitVec3Coord( position );
}

void CTapGesture::FromBitBuffer( bf_read *buf )
{
	handId = buf->ReadVarInt32();
	fingerId = buf->ReadVarInt32();
	buf->ReadBitVec3Normal( direction );
	buf->ReadBitVec3Coord( position );
}

void CTapGesture::Transform( float yaw, const Vector &translation )
{
	// Apply rotations.
	VectorYawRotate( position, yaw, position );
	VectorYawRotate( direction, yaw, direction );

	// Apply translations.
	position += translation;
}

//=============================================================================
// CBallGesture implementation.
//=============================================================================
CBallGesture::CBallGesture()
{
	handId = INVALID_INDEX;
	radius = grabStrength = 0.0f;
	center = vec3_origin;
}

#ifdef CLIENT_DLL
CBallGesture::CBallGesture( const Leap::Hand &h )
{
	FromLeap( h );
}

void CBallGesture::FromLeap( const Leap::Hand &h )
{
	handId = h.id();
	radius = LeapToSourceDistance( h.sphereRadius() );
	grabStrength = h.grabStrength();
	center = LeapToSourceVector( h.sphereCenter(), true );
}
#endif

void CBallGesture::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteSignedVarInt32( handId );
	buf->WriteFloat( radius );
	buf->WriteFloat( grabStrength );
	buf->WriteBitVec3Coord( center );
}

void CBallGesture::FromBitBuffer( bf_read *buf )
{
	handId = buf->ReadSignedVarInt32();
	radius = buf->ReadFloat();
	grabStrength = buf->ReadFloat();
	buf->ReadBitVec3Coord( center );
}

void CBallGesture::Transform( float yaw, const Vector &translation )
{
	// Apply rotations.
	VectorYawRotate( center, yaw, center );

	// Apply translations.
	center += translation;
}

//=============================================================================
// CFrame implementation.
//=============================================================================
CFrame::CFrame()
{
	_gestureBits = 0;
}

#ifdef CLIENT_DLL
CFrame::CFrame( const Leap::Frame &f )
{
	FromLeap( f );
}

void CFrame::FromLeap( const Leap::Frame &f )
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
			_circle = CCircleGesture( gesture );
			SetGestureActive( EGesture::GESTURE_CIRCLE );
		}
		else if ( gesture.type() == Leap::Gesture::TYPE_SWIPE )
		{
			_swipe = CSwipeGesture( gesture );
			SetGestureActive( EGesture::GESTURE_SWIPE );
		}
		else if ( gesture.type() == Leap::Gesture::TYPE_KEY_TAP )
		{
			Leap::KeyTapGesture tap( gesture );
			_tap = CTapGesture( tap );
			SetGestureActive( EGesture::GESTURE_TAP );
		}
		else if (gesture.type() == Leap::Gesture::TYPE_SCREEN_TAP )
		{
			Leap::ScreenTapGesture tap( gesture );
			_tap = CTapGesture( tap );
			SetGestureActive( EGesture::GESTURE_TAP );
		}
	}

	if (!hands.isEmpty())
	{
		_hand = CHand( hands[0] );
		_ball = CBallGesture( hands[0] );
	}
}
#endif

void CFrame::ToBitBuffer( bf_write *buf ) const
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

void CFrame::FromBitBuffer( bf_read *buf )
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
void CFrame::ToEntitySpace( CBaseCombatCharacter *entity, const Vector &delta )
{
	// Convert the players direction vector to angles.
	QAngle ownerAngles;
	VectorAngles( entity->BodyDirection2D(), ownerAngles );

	Vector translation = entity->GetAbsOrigin() + delta;
	const float yaw = ownerAngles.y;

	_hand.Transform( yaw, translation );
	_ball.Transform( yaw, translation );
	_circle.Transform( yaw, translation );
	_swipe.Transform( yaw, translation );
	_tap.Transform( yaw, translation );
}
#endif