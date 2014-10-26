/*
===============================================================================

	holo_shared.cpp
		Code\structures that are to be shared between the client and server.

===============================================================================
*/

#include "cbase.h"
#include "holo_shared.h"

using namespace std;

static ConVar holo_arm_length( "holo_arm_length", "650", FCVAR_ARCHIVE, "Users arm length in mm" );

//===============================================================================
// Leap -> Source conversion.
//===============================================================================
#ifdef CLIENT_DLL
	Vector			LeapToSourceVector( const GiantLeap::Vector &v, bool transform = false );
	EFinger			LeapToSourceFingerCode( const GiantLeap::Finger::Type &finger );
	EBone			LeapToSourceBoneCode( const GiantLeap::Bone::Type &bone );
	float			LeapToSourceDistance( float distance );

	GiantLeap::Bone::Type	SourceToLeapBoneCode( EBone bone );
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
const char *EHandToString( EHand hand )
{
	switch( hand )
	{
		case HAND_LEFT:
			return "HAND_LEFT";

		case HAND_RIGHT:
			return "HAND_RIGHT";

		default:
			break;
	}

	return "HAND_COUNT";
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const char *EWorldPanelToString( EWorldPanel panel )
{
	switch( panel )
	{
		case WORLD_PANEL_LEFT:
			return "WORLD_PANEL_LEFT";

		case WORLD_PANEL_MIDDLE:
			return "WORLD_PANEL_MIDDLE";

		case WORLD_PANEL_RIGHT:
			return "WORLD_PANEL_RIGHT";

		default:
			break;
	}

	return "WORLD_PANEL_COUNT";
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
Vector LeapToSourceVector( const GiantLeap::Vector &v, bool transform /*= false*/ )
{
	GiantLeap::Vector transformed = v;
	Vector ov;

	if( transform )
	{
		// Set the origin of the Leap space to be 20cm above, and 50cm behind the device.
		transformed += GiantLeap::Vector( 0, -200, -holo_arm_length.GetInt() );
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

EFinger LeapToSourceFingerCode( const GiantLeap::Finger::Type &finger )
{
	switch( finger )
	{
		case GiantLeap::Finger::TYPE_THUMB:
			return EFinger::FINGER_THUMB;

		case GiantLeap::Finger::TYPE_INDEX:
			return EFinger::FINGER_POINTER;

		case GiantLeap::Finger::TYPE_MIDDLE:
			return EFinger::FINGER_MIDDLE;

		case GiantLeap::Finger::TYPE_RING:
			return EFinger::FINGER_RING;

		case GiantLeap::Finger::TYPE_PINKY:
			return EFinger::FINGER_PINKY;

		default:
			break;
	}

	Assert( 0 );
	return EFinger::FINGER_COUNT;
}

EBone LeapToSourceBoneCode( const GiantLeap::Bone::Type &bone )
{
	switch( bone )
	{
		case GiantLeap::Bone::TYPE_METACARPAL:
			return EBone::BONE_METACARPAL;

		case GiantLeap::Bone::TYPE_PROXIMAL:
			return EBone::BONE_PROXIMAL;

		case GiantLeap::Bone::TYPE_INTERMEDIATE:
			return EBone::BONE_INTERMEDIATE;

		case GiantLeap::Bone::TYPE_DISTAL:
			return EBone::BONE_DISTAL;

		default:
			break;
	}

	Assert( 0 );
	return EBone::BONE_COUNT;
}

GiantLeap::Bone::Type SourceToLeapBoneCode( EBone bone )
{
	switch( bone )
	{
		case EBone::BONE_METACARPAL:
			return GiantLeap::Bone::TYPE_METACARPAL;

		case EBone::BONE_PROXIMAL:
			return  GiantLeap::Bone::TYPE_PROXIMAL;

		case EBone::BONE_INTERMEDIATE:
			return GiantLeap::Bone::TYPE_INTERMEDIATE;

		case EBone::BONE_DISTAL:
			return GiantLeap::Bone::TYPE_DISTAL;

		default:
			break;
	}

	Assert( 0 );
	return GiantLeap::Bone::TYPE_METACARPAL;
}

#endif

//=============================================================================
// CArm implementation.
//=============================================================================
CArm::CArm()
{
	_wristPosition = _elbowPosition = vec3_origin;
}

#ifdef CLIENT_DLL

CArm::CArm( const GiantLeap::Arm &a )
{
	FromLeap( a );
}

void CArm::FromLeap( const GiantLeap::Arm &a )
{
	_wristPosition = LeapToSourceVector( a.wristPosition(), true );
	_elbowPosition = LeapToSourceVector( a.elbowPosition(), true );
}

#endif

void CArm::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteBitVec3Coord( _wristPosition );
	buf->WriteBitVec3Coord( _elbowPosition );
}

void CArm::FromBitBuffer( bf_read *buf )
{
	buf->ReadBitVec3Coord( _wristPosition );
	buf->ReadBitVec3Coord( _elbowPosition );
}

void CArm::Transform( float yaw, const Vector &translation )
{
	// Apply rotations.
	VectorYawRotate( _wristPosition, yaw, _wristPosition );
	VectorYawRotate( _elbowPosition, yaw, _elbowPosition );

	// Apply translations.
	_wristPosition += translation;
	_elbowPosition += translation;
}

CArm CArm::operator+( const CArm &other ) const
{
	CArm a;

	a._wristPosition = _wristPosition + other._wristPosition;
	a._elbowPosition = _elbowPosition + other._elbowPosition;

	return a;
}

CArm CArm::operator/( float scale ) const
{
	CArm a;

	a._wristPosition = _wristPosition / scale;
	a._elbowPosition = _elbowPosition / scale;

	return a;
}

CArm CArm::operator*( float scale ) const
{
	CArm a;

	a._wristPosition = _wristPosition * scale;
	a._elbowPosition = _elbowPosition * scale;

	return a;
}


//=============================================================================
// CBone implementation.
//=============================================================================
CBone::CBone()
{
	_nextJoint = _prevJoint = vec3_origin;
}

#ifdef CLIENT_DLL

CBone::CBone(const GiantLeap::Bone &b)
{
	FromLeap( b );
}

void CBone::FromLeap(const GiantLeap::Bone &b)
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

CBone CBone::operator+( const CBone &other ) const
{
	CBone b;

	b._nextJoint = _nextJoint + other._nextJoint;
	b._prevJoint = _prevJoint + other._prevJoint;

	return b;
}

CBone CBone::operator/( float scale ) const
{
	CBone b;

	b._nextJoint = _nextJoint / scale;
	b._prevJoint = _prevJoint / scale;

	return b;
}

CBone CBone::operator*( float scale ) const
{
	CBone b;

	b._nextJoint = _nextJoint * scale;
	b._prevJoint = _prevJoint * scale;

	return b;
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
CFinger::CFinger( const GiantLeap::Finger &f )
{
	FromLeap( f );
}

void CFinger::FromLeap( const GiantLeap::Finger &f )
{
	_id = f.id();
	_type = LeapToSourceFingerCode( f.type() );
	_direction = LeapToSourceVector( f.direction() );
	_tipPosition = LeapToSourceVector( f.tipPosition(), true );
	_tipVelocity = LeapToSourceVector( f.tipVelocity() );
	_width = LeapToSourceDistance( f.width() );
	_length = LeapToSourceDistance( f.length() );

	_direction.NormalizeInPlace();

	for( int i = 0; i < EBone::BONE_COUNT; i++ )
	{
		GiantLeap::Bone::Type leapBone = SourceToLeapBoneCode( (EBone)i );
		_bones[i].FromLeap( f.bone( leapBone ) );
	}
}
#endif

void CFinger::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteVarInt32( _id );
	buf->WriteChar( _type );
	buf->WriteBitVec3Normal( _direction );
	buf->WriteBitVec3Coord( _tipPosition );
	buf->WriteBitVec3Coord( _tipVelocity );
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
	_type = buf->ReadChar();
	buf->ReadBitVec3Normal( _direction );
	buf->ReadBitVec3Coord( _tipPosition );
	buf->ReadBitVec3Coord( _tipVelocity );
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
	VectorYawRotate( _tipVelocity, yaw, _tipVelocity );
	VectorYawRotate( _tipPosition, yaw, _tipPosition );
	VectorYawRotate( _direction, yaw, _direction );

	// Apply translations.
	_tipPosition += translation;

	for( int i = 0; i < EBone::BONE_COUNT; i++ )
	{
		_bones[i].Transform( yaw, translation );
	}
}

float CFinger::GetVelocityDirectionTheta() const
{
	const Vector norm = _direction.Normalized();
	float dot = _tipVelocity.Normalized().Dot( norm );
	float delta = acos( dot );
	return RAD2DEG( delta );
}

CFinger CFinger::operator+( const CFinger &other ) const
{
	CFinger f;

	f._id = _id;
	f._type = _type;
	f._direction = _direction + other._direction;
	f._tipPosition = _tipPosition + other._tipPosition;
	f._tipVelocity = _tipVelocity + other._tipVelocity;
	f._width = _width + other._width;
	f._length = _length + other._length;

	for( int i = 0; i < EBone::BONE_COUNT; i++ )
	{
		f._bones[i] = _bones[i] + other._bones[i];
	}

	return f;
}

CFinger CFinger::operator/( float scale ) const
{
	CFinger f;

	f._id = _id;
	f._type = _type;
	f._direction = _direction / scale;
	f._tipPosition = _tipPosition / scale;
	f._tipVelocity = _tipVelocity / scale;
	f._width = _width / scale;
	f._length = _length / scale;

	for( int i = 0; i < EBone::BONE_COUNT; i++ )
	{
		f._bones[i] = _bones[i] / scale;
	}

	return f;
}

CFinger CFinger::operator*( float scale ) const
{
	CFinger f;

	f._id = _id;
	f._type = _type;
	f._direction = _direction * scale;
	f._tipPosition = _tipPosition * scale;
	f._tipVelocity = _tipVelocity * scale;
	f._width = _width * scale;
	f._length = _length * scale;

	for( int i = 0; i < EBone::BONE_COUNT; i++ )
	{
		f._bones[i] = _bones[i] * scale;
	}

	return f;
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
CHand::CHand( const GiantLeap::Hand &h )
{
	FromLeap( h );
}

void CHand::FromLeap( const GiantLeap::Hand &h )
{
	BuildFingers( h );

	_id = h.id();
	_ball.FromLeap( h );
	_confidence = h.confidence();
	_pinchStrength = h.pinchStrength();
	_velocity = LeapToSourceVector( h.palmVelocity() );
	_normal = LeapToSourceVector( h.palmNormal() );
	_direction = LeapToSourceVector( h.direction() );
	_position = LeapToSourceVector( h.palmPosition(), true );

	_normal.NormalizeInPlace();
	_direction.NormalizeInPlace();

	_arm.FromLeap( h.arm() );
}

void CHand::BuildFingers( const GiantLeap::Hand &h )
{
	const GiantLeap::FingerList &fingerlist = h.fingers();
	for each( const GiantLeap::Finger &f in fingerlist )
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
	buf->WriteBitVec3Coord( _velocity );

	for( int i = 0; i < EFinger::FINGER_COUNT; i++ )
	{
		_fingers[i].ToBitBuffer( buf );
	}

	_ball.ToBitBuffer( buf );
	_arm.ToBitBuffer( buf );
}

void CHand::FromBitBuffer( bf_read *buf )
{
	_id = buf->ReadVarInt32();
	_confidence = buf->ReadFloat();
	_pinchStrength = buf->ReadFloat();
	buf->ReadBitVec3Normal( _direction );
	buf->ReadBitVec3Normal( _normal );
	buf->ReadBitVec3Coord( _position );
	buf->ReadBitVec3Coord( _velocity );

	for( int i = 0; i < EFinger::FINGER_COUNT; i++ )
	{
		_fingers[i].FromBitBuffer( buf );
	}

	_ball.FromBitBuffer( buf );
	_arm.FromBitBuffer( buf );
}

void CHand::Transform( float yaw, const Vector &translation )
{
	const Vector extraTranslation = translation + Vector( 0.0f, 0.0f, _position.z * 1.0f );
	
	_ball.Transform( yaw, extraTranslation );

	// Apply rotations.
	VectorYawRotate( _position, yaw, _position );
	VectorYawRotate( _normal, yaw, _normal );
	VectorYawRotate( _direction, yaw, _direction );
	VectorYawRotate( _velocity, yaw, _velocity );
	
	// Apply translations.
	_position += extraTranslation;
	
	// Transform fingers.
	for( int i = 0; i < FINGER_COUNT; i++ )
	{
		_fingers[i].Transform( yaw, extraTranslation );
	}

	_arm.Transform( yaw, extraTranslation );
}

float CHand::FindThetaBetweenFingers( EFinger f1, EFinger f2 ) const
{
	const CFinger &finger1 = GetFingerByType( f1 );
	const CFinger &finger2 = GetFingerByType( f2 );

	const Vector &f1dir = finger1.GetDirection();
	const Vector &f2dir = finger2.GetDirection();

	float dot = f1dir.Dot( f2dir );
	float mag = f1dir.Length() * f2dir.Length();
	float theta = acos( dot / mag );

	return RAD2DEG( theta );
}

const CFinger *CHand::GetFingerById( int id ) const
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

const CFinger &CHand::GetClosestFingerTo( EFinger to ) const
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

CHand CHand::operator+( const CHand &other ) const
{
	CHand h;

	h._id = _id;
	h._confidence = _confidence + other._confidence;
	h._pinchStrength = _pinchStrength + other._pinchStrength;
	h._direction = _direction + other._direction;
	h._normal = _normal + other._normal;
	h._position = _position + other._position;
	h._velocity = _velocity + other._velocity;
	h._ball = _ball + other._ball;

	for( int i = 0; i < EFinger::FINGER_COUNT; i++ )
	{
		h._fingers[i] = _fingers[i] + other._fingers[i];
	}

	h._arm = _arm + other._arm;

	return h;
}

CHand CHand::operator/( float scale ) const
{
	CHand h;

	h._id = _id;
	h._confidence = _confidence / scale;
	h._pinchStrength = _pinchStrength / scale;
	h._direction = _direction / scale;
	h._normal = _normal / scale;
	h._position = _position / scale;
	h._velocity = _velocity / scale;
	h._ball = _ball / scale;

	for( int i = 0; i < EFinger::FINGER_COUNT; i++ )
	{
		h._fingers[i] = _fingers[i] / scale;
	}

	h._arm = _arm / scale;

	return h;
}

CHand CHand::operator*( float scale ) const
{
	CHand h;

	h._id = _id;
	h._confidence = _confidence * scale;
	h._pinchStrength = _pinchStrength * scale;
	h._direction = _direction * scale;
	h._normal = _normal * scale;
	h._position = _position * scale;
	h._velocity = _velocity * scale;
	h._ball = _ball * scale;

	for( int i = 0; i < EFinger::FINGER_COUNT; i++ )
	{
		h._fingers[i] = _fingers[i] * scale;
	}

	h._arm = _arm * scale;

	return h;
}

//=============================================================================
// CCircleGesture implementation.
//=============================================================================
CCircleGesture::CCircleGesture()
{
	_handType = HAND_INVALID;
	_fingerType = FINGER_INVALID;
	_radius = _duration = 0.0f;
	_center = _normal = vec3_origin;
	_clockwise = false;
}

#ifdef CLIENT_DLL
CCircleGesture::CCircleGesture( const GiantLeap::CircleGesture &c )
{
	FromLeap( c );
}

void CCircleGesture::FromLeap( const GiantLeap::CircleGesture &c )
{
	//const GiantLeap::HandList &hands = c.hands();
	//const GiantLeap::Finger finger = GiantLeap::Finger( c.pointable() );

	//_handType = hands[0].isLeft() ? HAND_LEFT : HAND_RIGHT;
	//_fingerType = LeapToSourceFingerCode( finger.type() );
	_handType = HAND_RIGHT;
	_fingerType = FINGER_POINTER;
	_radius = LeapToSourceDistance( c.radius() );
	_center = LeapToSourceVector( c.center(), true );
	_normal = LeapToSourceVector( c.normal() );
	_duration = c.durationSeconds();
	_progress = c.progress();

	_normal.NormalizeInPlace();

	// According to the Leap SDK:
	// If you draw the circle clockwise, the normal vector points in the same general direction as the pointable object drawing the circle.
	// If you draw the circle counterclockwise, the normal points back toward the pointable.
	//
	// Therefore if the angle to the normal (from the pointable) is less than 90 degrees (PI/2 radians) the directions are roughly the same,
	// which means the circle is clockwise.
	//const GiantLeap::Vector &pointableDir = c.pointable().direction();
	//_clockwise = pointableDir.angleTo( c.normal() ) <= ( M_PI / 2 );
	_clockwise = c.clockwise();
}
#endif

void CCircleGesture::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteVarInt32( _handType );
	buf->WriteVarInt32( _fingerType );
	buf->WriteFloat( _radius );
	buf->WriteFloat( _duration );
	buf->WriteBitVec3Coord( _center );
	buf->WriteBitVec3Normal( _normal );
	buf->WriteChar( _clockwise );
	buf->WriteFloat( _progress );
}

void CCircleGesture::FromBitBuffer( bf_read *buf )
{
	_handType = (EHand)buf->ReadVarInt32();
	_fingerType = (EFinger)buf->ReadVarInt32();
	_radius = buf->ReadFloat();
	_duration = buf->ReadFloat();
	buf->ReadBitVec3Coord( _center );
	buf->ReadBitVec3Normal( _normal );
	_clockwise = buf->ReadChar() != 0 ? true : false;
	_progress = buf->ReadFloat();
}

void CCircleGesture::Transform( float yaw, const Vector &translation )
{
	// Apply rotations.
	VectorYawRotate( _center, yaw, _center );
	VectorYawRotate( _normal, yaw, _normal );

	// Apply translation.
	_center += translation;
}

CCircleGesture CCircleGesture::operator+( const CCircleGesture &other ) const
{
	CCircleGesture g;

	g._handType = _handType;
	g._fingerType = _fingerType;
	g._radius = _radius + other._radius;
	g._duration = max( _duration, other._duration );
	g._center = _center + other._center;
	g._normal = _normal + other._normal;
	g._clockwise = _clockwise;
	g._progress = _progress + other._progress;

	return g;
}

CCircleGesture CCircleGesture::operator/( float scale ) const
{
	CCircleGesture g;

	g._handType = _handType;
	g._fingerType = _fingerType;
	g._duration = _duration;
	g._clockwise = _clockwise;
	g._radius = _radius / scale;
	g._center = _center / scale;
	g._normal = _normal / scale;
	g._progress = _progress / scale;

	return g;
}

CCircleGesture CCircleGesture::operator*( float scale ) const
{
	CCircleGesture g;

	g._handType = _handType;
	g._fingerType = _fingerType;
	g._duration = _duration;
	g._clockwise = _clockwise;
	g._radius = _radius * scale;
	g._center = _center * scale;
	g._normal = _normal * scale;
	g._progress = _progress * scale;

	return g;
}

//=============================================================================
// CSwipeGesture implementation.
//=============================================================================
CSwipeGesture::CSwipeGesture()
{
	_handType = HAND_INVALID;
	_speed = 0;
	_direction = _curPosition = _startPosition = vec3_origin;
}

#ifdef CLIENT_DLL
CSwipeGesture::CSwipeGesture( const GiantLeap::SwipeGesture &s )
{
	FromLeap( s );
}

void CSwipeGesture::FromLeap( const GiantLeap::SwipeGesture &s )
{
	const GiantLeap::HandList &hands = s.hands();

	_handType = hands[0].isLeft() ? HAND_LEFT : HAND_RIGHT;
	_speed = s.speed();
	_direction = LeapToSourceVector( s.direction() );
	_curPosition = LeapToSourceVector( s.position(), true );
	_startPosition = LeapToSourceVector( s.startPosition(), true );

	_direction.NormalizeInPlace();
}
#endif

void CSwipeGesture::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteVarInt32( _handType );
	buf->WriteFloat( _speed );
	buf->WriteBitVec3Normal( _direction );
	buf->WriteBitVec3Coord( _curPosition );
	buf->WriteBitVec3Coord( _startPosition );
}

void CSwipeGesture::FromBitBuffer( bf_read *buf )
{
	_handType = (EHand)buf->ReadVarInt32();
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

CSwipeGesture CSwipeGesture::operator+( const CSwipeGesture &other ) const
{
	CSwipeGesture g;

	g._handType = _handType;
	g._speed = _speed + other._speed;
	g._direction = _direction + other._direction;
	g._curPosition = _curPosition + other._curPosition;
	g._startPosition = _startPosition + other._startPosition;

	return g;
}

CSwipeGesture CSwipeGesture::operator/( float scale ) const
{
	CSwipeGesture g;

	g._handType = _handType;
	g._speed = _speed / scale;
	g._direction = _direction / scale;
	g._curPosition = _curPosition / scale;
	g._startPosition = _startPosition / scale;

	return g;
}

CSwipeGesture CSwipeGesture::operator*( float scale ) const
{
	CSwipeGesture g;

	g._handType = _handType;
	g._speed = _speed * scale;
	g._direction = _direction * scale;
	g._curPosition = _curPosition * scale;
	g._startPosition = _startPosition * scale;

	return g;
}

//=============================================================================
// CTapGesture implementation.
//=============================================================================
CTapGesture::CTapGesture()
{
	_handType = HAND_INVALID;
	_fingerType = FINGER_INVALID;
	_direction = _position = vec3_origin;
}

#ifdef CLIENT_DLL
CTapGesture::CTapGesture( const GiantLeap::KeyTapGesture &k )
{
	FromLeap( k );
}

CTapGesture::CTapGesture( const GiantLeap::ScreenTapGesture &s )
{
	FromLeap( s );
}

void CTapGesture::FromLeap( const GiantLeap::KeyTapGesture &k )
{
	const GiantLeap::HandList &hands = k.hands();
	const GiantLeap::Finger finger = GiantLeap::Finger( k.pointable() );

	_handType = hands[0].isLeft() ? HAND_LEFT : HAND_RIGHT;
	_fingerType = LeapToSourceFingerCode( finger.type() );
	_direction = LeapToSourceVector( k.direction() );
	_position = LeapToSourceVector( k.position(), true );

	_direction.NormalizeInPlace();
}

void CTapGesture::FromLeap( const GiantLeap::ScreenTapGesture &s )
{
	const GiantLeap::HandList &hands = s.hands();
	const GiantLeap::Finger finger = GiantLeap::Finger( s.pointable() );

	_handType = hands[0].isLeft() ? HAND_LEFT : HAND_RIGHT;
	_fingerType = LeapToSourceFingerCode( finger.type() );
	_direction = LeapToSourceVector( s.direction() );
	_position = LeapToSourceVector( s.position(), true );

	_direction.NormalizeInPlace();
}
#endif

void CTapGesture::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteVarInt32( _handType );
	buf->WriteVarInt32( _fingerType );
	buf->WriteBitVec3Normal( _direction );
	buf->WriteBitVec3Coord( _position );
}

void CTapGesture::FromBitBuffer( bf_read *buf )
{
	_handType = (EHand)buf->ReadVarInt32();
	_fingerType = (EFinger)buf->ReadVarInt32();
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

CTapGesture CTapGesture::operator+( const CTapGesture &other ) const
{
	CTapGesture g;

	g._handType = _handType;
	g._fingerType = _fingerType;
	g._direction = _direction + other._direction;
	g._position = _position + other._position;

	return g;
}

CTapGesture CTapGesture::operator/( float scale ) const
{
	CTapGesture g;

	g._handType = _handType;
	g._fingerType = _fingerType;
	g._direction = _direction / scale;
	g._position = _position / scale;

	return g;
}

CTapGesture CTapGesture::operator*( float scale ) const
{
	CTapGesture g;

	g._handType = _handType;
	g._fingerType = _fingerType;
	g._direction = _direction * scale;
	g._position = _position * scale;

	return g;
}

//=============================================================================
// CBallGesture implementation.
//=============================================================================
CBallGesture::CBallGesture()
{
	_handType = HAND_INVALID;
	_radius = _grabStrength = 0.0f;
	_center = vec3_origin;
}

#ifdef CLIENT_DLL
CBallGesture::CBallGesture( const GiantLeap::Hand &h )
{
	FromLeap( h );
}

void CBallGesture::FromLeap( const GiantLeap::Hand &h )
{
	_handType = h.isLeft() ? HAND_LEFT : HAND_RIGHT;
	_radius = LeapToSourceDistance( h.sphereRadius() );
	_grabStrength = h.grabStrength();
	_center = LeapToSourceVector( h.sphereCenter(), true );
}
#endif

void CBallGesture::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteSignedVarInt32( _handType );
	buf->WriteFloat( _radius );
	buf->WriteFloat( _grabStrength );
	buf->WriteBitVec3Coord( _center );
}

void CBallGesture::FromBitBuffer( bf_read *buf )
{
	_handType = (EHand)buf->ReadSignedVarInt32();
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

CBallGesture CBallGesture::operator+( const CBallGesture &other ) const
{
	CBallGesture g;

	g._handType = _handType;
	g._radius = _radius + other._radius;
	g._grabStrength = _grabStrength + other._grabStrength;
	g._center = _center + other._center;

	return g;
}

CBallGesture CBallGesture::operator/( float scale ) const
{
	CBallGesture g;

	g._handType = _handType;
	g._radius = _radius / scale;
	g._grabStrength = _grabStrength / scale;
	g._center = _center / scale;

	return g;
}

CBallGesture CBallGesture::operator*( float scale ) const
{
	CBallGesture g;

	g._handType = _handType;
	g._radius = _radius * scale;
	g._grabStrength = _grabStrength * scale;
	g._center = _center * scale;

	return g;
}

//=============================================================================
// CFrame implementation.
//=============================================================================
CFrame::CFrame()
{
	_valid = false;
	_gestureBits = 0;
}

#ifdef CLIENT_DLL
CFrame::CFrame( const GiantLeap::Frame &f )
{
	FromLeap( f );
}

void CFrame::FromLeap( const GiantLeap::Frame &f )
{
	const GiantLeap::GestureList &gestures = f.gestures();
	const GiantLeap::HandList &hands = f.hands();

	_gestureBits = 0;

	for( int i = 0; i < gestures.count(); i++ )
	{
		const GiantLeap::Gesture &gesture = gestures[i];
		string data;

		if ( gesture.type() == GiantLeap::Gesture::TYPE_CIRCLE )
		{
			_circle = CCircleGesture( gesture );
			SetGestureActive( EGesture::GESTURE_CIRCLE );
		}
		else if ( gesture.type() == GiantLeap::Gesture::TYPE_SWIPE )
		{
			_swipe = CSwipeGesture( gesture );
			SetGestureActive( EGesture::GESTURE_SWIPE );
		}
		else if ( gesture.type() == GiantLeap::Gesture::TYPE_KEY_TAP )
		{
			GiantLeap::KeyTapGesture tap( gesture );
			_tap = CTapGesture( tap );
			SetGestureActive( EGesture::GESTURE_TAP );
		}
		else if (gesture.type() == GiantLeap::Gesture::TYPE_SCREEN_TAP )
		{
			GiantLeap::ScreenTapGesture tap( gesture );
			_tap = CTapGesture( tap );
			SetGestureActive( EGesture::GESTURE_TAP );
		}
	}

	bool wroteLeft = false;
	bool wroteRight = false;
	for( int i = 0; i < hands.count(); i++ )
	{
		if( hands[i].isLeft() )
		{
			_hand[HAND_LEFT].FromLeap( hands[i] );
			wroteLeft = true;
		}
		else if( hands[i].isRight() )
		{
			_hand[HAND_RIGHT].FromLeap( hands[i] );
			wroteRight = true;
		}

		if( wroteLeft && wroteRight )
		{
			break;
		}
	}
}
#endif

void CFrame::ToBitBuffer( bf_write *buf ) const
{
	buf->WriteVarInt32( _valid );
	if( !_valid )
	{
		return;
	}

	_hand[HAND_LEFT].ToBitBuffer( buf );
	_hand[HAND_RIGHT].ToBitBuffer( buf );
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
	_valid = buf->ReadVarInt32() != 0 ? true : false;
	if( !_valid )
	{
		return;
	}

	_hand[HAND_LEFT].FromBitBuffer( buf );
	_hand[HAND_RIGHT].FromBitBuffer( buf );
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

// Transforms the frame data to be positioned relative to the given entity.
void CFrame::ToEntitySpace( CBaseCombatCharacter *entity, const Vector &delta )
{
	// Convert the players direction vector to angles.
	QAngle ownerAngles;
	VectorAngles( entity->BodyDirection2D(), ownerAngles );

	Vector translation = entity->GetAbsOrigin() + delta;
	const float yaw = ownerAngles.y;

	_hand[HAND_LEFT].Transform( yaw, translation );
	_hand[HAND_RIGHT].Transform( yaw, translation );

	if( IsGestureActive( EGesture::GESTURE_CIRCLE ) )
	{
		_circle.Transform( yaw, translation );
	}

	if( IsGestureActive( EGesture::GESTURE_SWIPE ) )
	{
		_swipe.Transform( yaw, translation );
	}

	if( IsGestureActive( EGesture::GESTURE_TAP ) )
	{
		_tap.Transform( yaw, translation );
	}
}

bool CFrame::IsValid() const
{
	if( _hand[HAND_LEFT].GetPosition().IsZero() && _hand[HAND_RIGHT].GetPosition().IsZero() )
	{
		return false;
	}

	return _valid;
}

const CHand *CFrame::GetHandById( int id ) const
{
	// Dont use this.
	Assert( 0 );

	if( _hand[HAND_LEFT].GetId() == id )
	{
		return &_hand[HAND_LEFT];
	}

	if( _hand[HAND_RIGHT].GetId() == id )
	{
		return &_hand[HAND_RIGHT];
	}

	return NULL;
}

CFrame CFrame::operator+( const CFrame &other ) const
{
	if( IsValid() && !other.IsValid() )
	{
		return *this; 
	}
	else if( !IsValid() && other.IsValid() )
	{
		return other;
	}
	else if( !IsValid() && !other.IsValid() )
	{
		return CFrame();
	}

	// Both frames are valid. Add them!
	CFrame f;

	f._hand[HAND_LEFT] = _hand[HAND_LEFT] + other._hand[HAND_LEFT];
	f._hand[HAND_RIGHT] = _hand[HAND_RIGHT] + other._hand[HAND_RIGHT];

	f._circle = AddGesture( other, _circle, other._circle, EGesture::GESTURE_CIRCLE );
	f._swipe = AddGesture( other, _swipe, other._swipe, EGesture::GESTURE_SWIPE );
	f._tap = AddGesture( other, _tap, other._tap, EGesture::GESTURE_TAP );

	f._valid = true;
	f._gestureBits = _gestureBits | other._gestureBits;

	return f;
}

CFrame CFrame::operator/( float scale ) const
{
	CFrame f;

	f._valid = _valid;
	f._gestureBits = _gestureBits;
	f._hand[HAND_LEFT] = _hand[HAND_LEFT] / scale;
	f._hand[HAND_RIGHT] = _hand[HAND_RIGHT] / scale;
	f._circle = _circle / scale;
	f._swipe = _swipe / scale;
	f._tap = _tap / scale;

	return f;
}

CFrame CFrame::operator*( float scale ) const
{
	CFrame f;

	f._valid = _valid;
	f._gestureBits = _gestureBits;
	f._hand[HAND_LEFT] = _hand[HAND_LEFT] * scale;
	f._hand[HAND_RIGHT] = _hand[HAND_RIGHT] * scale;
	f._circle = _circle * scale;
	f._swipe = _swipe * scale;
	f._tap = _tap * scale;

	return f;
}

template<class T> 
T CFrame::AddGesture( const CFrame &other, const T &g1, const T &g2, EGesture g ) const
{
	if( IsGestureActive( g ) && other.IsGestureActive( g ) )
	{
		return g1 + g2;
	}
	else if( IsGestureActive( g ) && !other.IsGestureActive( g ) )
	{
		return g1;
	}
	else if( !IsGestureActive( g ) && other.IsGestureActive( g ) )
	{
		return g2;
	}
	
	return T();
}