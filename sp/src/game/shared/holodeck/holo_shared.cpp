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
	EBone			LeapToSourceBoneCode( const Leap::Bone::Type &bone );
	float			LeapToSourceDistance( float distance );

	Leap::Bone::Type	SourceToLeapBoneCode( EBone bone );
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
const char *EBoneToString( EBone bone )
{
	switch( bone )
	{
		case BONE_METACARPAL:
			return "BONE_METACARPAL";

		case BONE_PROXIMAL:
			return "BONE_PROXIMAL";

		case BONE_INTERMEDIATE:
			return "BONE_INTERMEDIATE";

		case BONE_DISTAL:
			return "BONE_DISTAL";

		case BONE_COUNT:
			return "BONE_COUNT";

		default:
			break;
	}

	return "BONE_INVALID";
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

EBone LeapToSourceBoneCode( const Leap::Bone::Type &bone )
{
	switch( bone )
	{
		case Leap::Bone::TYPE_METACARPAL:
			return EBone::BONE_METACARPAL;

		case Leap::Bone::TYPE_PROXIMAL:
			return EBone::BONE_PROXIMAL;

		case Leap::Bone::TYPE_INTERMEDIATE:
			return EBone::BONE_INTERMEDIATE;

		case Leap::Bone::TYPE_DISTAL:
			return EBone::BONE_DISTAL;

		default:
			break;
	}

	Assert( 0 );
	return EBone::BONE_COUNT;
}

Leap::Bone::Type SourceToLeapBoneCode( EBone bone )
{
	switch( bone )
	{
		case EBone::BONE_METACARPAL:
			return Leap::Bone::TYPE_METACARPAL;

		case EBone::BONE_PROXIMAL:
			return  Leap::Bone::TYPE_PROXIMAL;

		case EBone::BONE_INTERMEDIATE:
			return Leap::Bone::TYPE_INTERMEDIATE;

		case EBone::BONE_DISTAL:
			return Leap::Bone::TYPE_DISTAL;

		default:
			break;
	}

	Assert( 0 );
	return Leap::Bone::TYPE_METACARPAL;
}

#endif


//=============================================================================
// CBone implementation.
//=============================================================================
CBone::CBone()
{
	_nextJoint = _prevJoint = vec3_origin;
}

#ifdef CLIENT_DLL

CBone::CBone(const Leap::Bone &b)
{
	FromLeap( b );
}

void CBone::FromLeap(const Leap::Bone &b)
{
	_nextJoint = LeapToSourceVector( b.nextJoint(), true );
	_prevJoint = LeapToSourceVector( b.prevJoint(), true );
}

#endif

void CBone::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteBitVec3Coord( _nextJoint );
	buf->WriteBitVec3Coord( _prevJoint );
}

void CBone::FromBitBuffer( bf_read *buf )
{
	buf->ReadBitVec3Coord( _nextJoint );
	buf->ReadBitVec3Coord( _prevJoint );
}

void CBone::Transform( float yaw, const Vector &translation )
{
	// Apply rotations.
	VectorYawRotate( _nextJoint, yaw, _nextJoint );
	VectorYawRotate( _prevJoint, yaw, _prevJoint );

	// Apply translations.
	_nextJoint += translation;
	_prevJoint += translation;
}

//=============================================================================
// CFinger implementation.
//=============================================================================
CFinger::CFinger()
{
	_id = -1;
	_width = _length = 0.0f;
	_direction = _tipPosition = _tipVelocity = vec3_origin;
}

#ifdef CLIENT_DLL
CFinger::CFinger( const Leap::Finger &f )
{
	FromLeap( f );
}

void CFinger::FromLeap( const Leap::Finger &f )
{
	_id = f.id();
	_direction = LeapToSourceVector( f.direction() );
	_tipPosition = LeapToSourceVector( f.tipPosition(), true );
	_tipVelocity = LeapToSourceVector( f.tipVelocity() );
	_width = LeapToSourceDistance( f.width() );
	_length = LeapToSourceDistance( f.length() );

	_direction.NormalizeInPlace();

	for( int i = 0; i < EBone::BONE_COUNT; i++ )
	{
		Leap::Bone::Type leapBone = SourceToLeapBoneCode( (EBone)i );
		_bones[i].FromLeap( f.bone( leapBone ) );
	}
}
#endif

void CFinger::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteVarInt32( _id );
	buf->WriteBitVec3Normal( _direction );
	buf->WriteBitVec3Coord( _tipPosition );
	buf->WriteBitVec3Normal( _tipVelocity );
	buf->WriteFloat( _width );
	buf->WriteFloat( _length );

	for( int i = 0; i < EBone::BONE_COUNT; i++ )
	{
		_bones[i].ToBitBuffer( buf );
	}
}

void CFinger::FromBitBuffer( bf_read *buf )
{
	_id = buf->ReadVarInt32();
	buf->ReadBitVec3Normal( _direction );
	buf->ReadBitVec3Coord( _tipPosition );
	buf->ReadBitVec3Normal( _tipVelocity );
	_width = buf->ReadFloat();
	_length = buf->ReadFloat();

	for( int i = 0; i < EBone::BONE_COUNT; i++ )
	{
		_bones[i].FromBitBuffer( buf );
	}
}

void CFinger::Transform( float yaw, const Vector &translation )
{
	// Apply rotations.
	VectorYawRotate( _tipPosition, yaw, _tipPosition );
	VectorYawRotate( _direction, yaw, _direction );

	// Apply translations.
	_tipPosition += translation;

	for( int i = 0; i < EBone::BONE_COUNT; i++ )
	{
		_bones[i].Transform( yaw, translation );
	}
}

//=============================================================================
// CHand implementation.
//=============================================================================
CHand::CHand()
{
	_id = INVALID_INDEX;
	_confidence = _pinchStrength = 0.0f;
	_direction = _position = _velocity = _normal = vec3_origin;
}

#ifdef CLIENT_DLL
CHand::CHand( const Leap::Hand &h )
{
	FromLeap( h );
}

void CHand::FromLeap( const Leap::Hand &h )
{
	BuildFingers( h );

	_id = h.id();
	_confidence = h.confidence();
	_pinchStrength = h.pinchStrength();
	_velocity = LeapToSourceVector( h.palmVelocity() );
	_normal = LeapToSourceVector( h.palmNormal() );
	_direction = LeapToSourceVector( h.direction() );
	_position = LeapToSourceVector( h.palmPosition(), true );

	_normal.NormalizeInPlace();
	_direction.NormalizeInPlace();
}

void CHand::BuildFingers( const Leap::Hand &h )
{
	const Leap::FingerList &fingerlist = h.fingers();
	for each( const Leap::Finger &f in fingerlist )
	{
		EFinger idx = LeapToSourceFingerCode( f.type() );
		_fingers[idx].FromLeap( f );
	}
}
#endif

void CHand::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteVarInt32( _id );
	buf->WriteFloat( _confidence );
	buf->WriteFloat( _pinchStrength );
	buf->WriteBitVec3Normal( _direction );
	buf->WriteBitVec3Normal( _normal );
	buf->WriteBitVec3Coord( _position );
	buf->WriteBitVec3Normal( _velocity );

	for( int i = 0; i < EFinger::FINGER_COUNT; i++ )
	{
		_fingers[i].ToBitBuffer( buf );
	}
}

void CHand::FromBitBuffer( bf_read *buf )
{
	_id = buf->ReadVarInt32();
	_confidence = buf->ReadFloat();
	_pinchStrength = buf->ReadFloat();
	buf->ReadBitVec3Normal( _direction );
	buf->ReadBitVec3Normal( _normal );
	buf->ReadBitVec3Coord( _position );
	buf->ReadBitVec3Normal( _velocity );

	for( int i = 0; i < EFinger::FINGER_COUNT; i++ )
	{
		_fingers[i].FromBitBuffer( buf );
	}
}

void CHand::Transform( float yaw, const Vector &translation )
{
	// Apply rotations.
	VectorYawRotate( _position, yaw, _position );
	VectorYawRotate( _normal, yaw, _normal );
	VectorYawRotate( _direction, yaw, _direction );

	// Apply translations.
	_position += translation;
	
	// Transform fingers.
	for( int i = 0; i < FINGER_COUNT; i++ )
	{
		_fingers[i].Transform( yaw, translation );
	}
}

inline const CFinger *CHand::GetFingerById( int id ) const
{
	for( int i = 0; i < FINGER_COUNT; i++ )
	{
		if( _fingers[i].GetId() == id )
		{
			return &_fingers[i];
		}
	}

	return NULL;
}

inline const CFinger &CHand::GetClosestFingerTo( EFinger to, EFinger f ) const
{
	EFinger closest = EFinger::FINGER_THUMB;
	float closestDistance = FLT_MAX;

	for( int i = 0; i < EFinger::FINGER_COUNT; i++ )
	{
		// Don't compare the target finger against itself.
		if( i == to )
			continue;

		Vector direction = _fingers[to].GetTipPosition() - _fingers[i].GetTipPosition();
		float distance = fabs( direction.LengthSqr() );

		if( distance < closestDistance )
		{
			closest = (EFinger)i;
			closestDistance = distance;
		}
	}

	return _fingers[closest];
}

//=============================================================================
// CCircleGesture implementation.
//=============================================================================
CCircleGesture::CCircleGesture()
{
	_handId = _fingerId = INVALID_INDEX;
	_radius = _duration = 0.0f;
	_center = _normal = vec3_origin;
}

#ifdef CLIENT_DLL
CCircleGesture::CCircleGesture( const Leap::CircleGesture &c )
{
	FromLeap( c );
}

void CCircleGesture::FromLeap( const Leap::CircleGesture &c )
{
	const Leap::HandList &hands = c.hands();

	_handId = hands[0].id();
	_fingerId = c.pointable().id();
	_radius = LeapToSourceDistance( c.radius() );
	_center = LeapToSourceVector( c.center(), true );
	_normal = LeapToSourceVector( c.normal() );
	_duration = c.durationSeconds();

	// According to the Leap SDK:
	// If you draw the circle clockwise, the normal vector points in the same general direction as the pointable object drawing the circle.
	// If you draw the circle counterclockwise, the normal points back toward the pointable.
	//
	// Therefore if the angle to the normal (from the pointable) is less than 90 degrees (PI/2 radians) the directions are roughly the same,
	// which means the circle is clockwise.
	const Leap::Vector &pointableDir = c.pointable().direction();
	_clockwise = pointableDir.angleTo( c.normal() ) <= ( M_PI / 2 );
}
#endif

void CCircleGesture::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteVarInt32( _handId );
	buf->WriteVarInt32( _fingerId );
	buf->WriteFloat( _radius );
	buf->WriteFloat( _duration );
	buf->WriteBitVec3Coord( _center );
	buf->WriteBitVec3Normal( _normal );
	buf->WriteVarInt32( _clockwise );
}

void CCircleGesture::FromBitBuffer( bf_read *buf )
{
	_handId = buf->ReadVarInt32();
	_fingerId = buf->ReadVarInt32();
	_radius = buf->ReadFloat();
	_duration = buf->ReadFloat();
	buf->ReadBitVec3Coord( _center );
	buf->ReadBitVec3Normal( _normal );
	_clockwise = buf->ReadVarInt32() != 0 ? true : false;
}

void CCircleGesture::Transform( float yaw, const Vector &translation )
{
	// Apply rotations.
	VectorYawRotate( _center, yaw, _center );
	VectorYawRotate( _normal, yaw, _normal );

	// Apply translation.
	_center += translation;
}

//=============================================================================
// CSwipeGesture implementation.
//=============================================================================
CSwipeGesture::CSwipeGesture()
{
	_handId = INVALID_INDEX;
	_speed = 0;
	_direction = _curPosition = _startPosition = vec3_origin;
}

#ifdef CLIENT_DLL
CSwipeGesture::CSwipeGesture( const Leap::SwipeGesture &s )
{
	FromLeap( s );
}

void CSwipeGesture::FromLeap( const Leap::SwipeGesture &s )
{
	const Leap::HandList &hands = s.hands();

	_handId = hands[0].id();
	_speed = s.speed();
	_direction = LeapToSourceVector( s.direction() );
	_curPosition = LeapToSourceVector( s.position(), true );
	_startPosition = LeapToSourceVector( s.startPosition(), true );
}
#endif

void CSwipeGesture::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteVarInt32( _handId );
	buf->WriteFloat( _speed );
	buf->WriteBitVec3Normal( _direction );
	buf->WriteBitVec3Coord( _curPosition );
	buf->WriteBitVec3Coord( _startPosition );
}

void CSwipeGesture::FromBitBuffer( bf_read *buf )
{
	_handId = buf->ReadVarInt32();
	_speed = buf->ReadFloat();
	buf->ReadBitVec3Normal( _direction );
	buf->ReadBitVec3Coord( _curPosition );
	buf->ReadBitVec3Coord( _startPosition );
}

void CSwipeGesture::Transform( float yaw, const Vector &translation )
{
	// Apply rotations.
	VectorYawRotate( _direction, yaw, _direction );
	VectorYawRotate( _curPosition, yaw, _curPosition );
	VectorYawRotate( _startPosition, yaw, _startPosition );

	// Apply translations.
	_curPosition += translation;
	_startPosition += translation;
}

//=============================================================================
// CTapGesture implementation.
//=============================================================================
CTapGesture::CTapGesture()
{
	_handId = _fingerId = INVALID_INDEX;
	_direction = _position = vec3_origin;
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

	_handId = hands[0].id();
	_fingerId = k.pointable().id();
	_direction = LeapToSourceVector( k.direction() );
	_position = LeapToSourceVector( k.position(), true );
}

void CTapGesture::FromLeap( const Leap::ScreenTapGesture &s )
{
	const Leap::HandList &hands = s.hands();

	_handId = hands[0].id();
	_fingerId = s.pointable().id();
	_direction = LeapToSourceVector( s.direction() );
	_position = LeapToSourceVector( s.position(), true );
}
#endif

void CTapGesture::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteVarInt32( _handId );
	buf->WriteVarInt32( _fingerId );
	buf->WriteBitVec3Normal( _direction );
	buf->WriteBitVec3Coord( _position );
}

void CTapGesture::FromBitBuffer( bf_read *buf )
{
	_handId = buf->ReadVarInt32();
	_fingerId = buf->ReadVarInt32();
	buf->ReadBitVec3Normal( _direction );
	buf->ReadBitVec3Coord( _position );
}

void CTapGesture::Transform( float yaw, const Vector &translation )
{
	// Apply rotations.
	VectorYawRotate( _position, yaw, _position );
	VectorYawRotate( _direction, yaw, _direction );

	// Apply translations.
	_position += translation;
}

//=============================================================================
// CBallGesture implementation.
//=============================================================================
CBallGesture::CBallGesture()
{
	_handId = INVALID_INDEX;
	_radius = _grabStrength = 0.0f;
	_center = vec3_origin;
}

#ifdef CLIENT_DLL
CBallGesture::CBallGesture( const Leap::Hand &h )
{
	FromLeap( h );
}

void CBallGesture::FromLeap( const Leap::Hand &h )
{
	_handId = h.id();
	_radius = LeapToSourceDistance( h.sphereRadius() );
	_grabStrength = h.grabStrength();
	_center = LeapToSourceVector( h.sphereCenter(), true );
}
#endif

void CBallGesture::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteSignedVarInt32( _handId );
	buf->WriteFloat( _radius );
	buf->WriteFloat( _grabStrength );
	buf->WriteBitVec3Coord( _center );
}

void CBallGesture::FromBitBuffer( bf_read *buf )
{
	_handId = buf->ReadSignedVarInt32();
	_radius = buf->ReadFloat();
	_grabStrength = buf->ReadFloat();
	buf->ReadBitVec3Coord( _center );
}

void CBallGesture::Transform( float yaw, const Vector &translation )
{
	// Apply rotations.
	VectorYawRotate( _center, yaw, _center );

	// Apply translations.
	_center += translation;
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

inline const CHand *CFrame::GetHandById( int id ) const
{
	return _hand.GetId() == id ? &_hand : NULL;
}