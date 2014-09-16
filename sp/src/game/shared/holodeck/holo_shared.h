/*
===============================================================================

	holo_shared.h
		Code\structures that are to be shared between the client and server.

===============================================================================
*/

#ifndef __HOLO_SHARED_H__
#define __HOLO_SHARED_H__

#include <istream>

#ifdef CLIENT_DLL
	// Required for custom structure constructors.
	#include <GiantLeap.h>
#endif

namespace holo
{

//=============================================================================
// Constants.
//=============================================================================
	enum EFinger
	{
		FINGER_THUMB,
		FINGER_POINTER,
		FINGER_MIDDLE,
		FINGER_RING,
		FINGER_PINKY,

		FINGER_COUNT
	};
	const char *EFingerToString( EFinger finger );

	enum EGesture
	{
		GESTURE_CIRCLE,
		GESTURE_SWIPE,
		GESTURE_TAP,

		GESTURE_COUNT
	};
	const char *EGestureToString( EGesture gesture );

	enum EBone
	{
		BONE_METACARPAL,
		BONE_PROXIMAL,
		BONE_INTERMEDIATE,
		BONE_DISTAL,

		BONE_COUNT
	};
	const char *EBoneToString( EBone bone );

	enum EGlobals
	{
		INVALID_INDEX = -1
	};
	const char *EGlobalsToString( EGlobals global );

//=============================================================================
// Classes.
//=============================================================================
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CArm
	{
	public:
						CArm();
#ifdef CLIENT_DLL
						CArm( const GiantLeap::Arm &b );
		void			FromLeap( const GiantLeap::Arm &b );
#endif

		void			ToBitBuffer( bf_write *buf ) const;
		void			FromBitBuffer( bf_read *buf );

		void			Transform( float yaw, const Vector &translation );

		inline const Vector &	GetWristPosition() const			{ return _wristPosition; }
		inline const Vector &	GetElbowPosition() const			{ return _elbowPosition; }
		inline Vector	GetDirection() const						{ return ( _elbowPosition - _wristPosition ).Normalized(); }
		inline float	GetLength() const							{ return fabs( ( _elbowPosition - _wristPosition ).Length() ); }

		// Filtering helpers.
		CArm			operator+( const CArm &other ) const;
		CArm			operator/( float scale ) const;
		CArm			operator*( float scale ) const;

	private:
		Vector			_wristPosition;
		Vector			_elbowPosition;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CBone
	{
	public:
						CBone();
#ifdef CLIENT_DLL
						CBone( const GiantLeap::Bone &b );
		void			FromLeap( const GiantLeap::Bone &b );
#endif

		void			ToBitBuffer( bf_write *buf ) const;
		void			FromBitBuffer( bf_read *buf );

		void			Transform( float yaw, const Vector &translation );

		// Accessors.
		inline const Vector &	GetNextJoint() const				{ return _nextJoint; }
		inline const Vector &	GetPrevJoint() const				{ return _prevJoint; }
		inline Vector	GetDirection() const						{ return ( _nextJoint - _prevJoint ).Normalized(); }
		inline Vector	GetCenter() const							{ return ( _nextJoint + _prevJoint ) / 2; }

		// Filtering helpers.
		CBone			operator+( const CBone &other ) const;
		CBone			operator/( float scale ) const;
		CBone			operator*( float scale ) const;

	private:
		Vector			_nextJoint;
		Vector			_prevJoint;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CFinger
	{
	public:
						CFinger();
#ifdef CLIENT_DLL
						CFinger( const GiantLeap::Finger &f );
		void			FromLeap( const GiantLeap::Finger &f );
#endif

		void			ToBitBuffer( bf_write *buf ) const;
		void			FromBitBuffer( bf_read *buf );

		void			Transform( float yaw, const Vector &translation );

		// Accessors.
		inline int		GetId() const								{ return _id; }
		inline const Vector &	GetDirection() const				{ return _direction; }
		inline const Vector &	GetTipPosition() const				{ return _tipPosition; }
		inline const Vector &	GetTipVelocity() const				{ return _tipVelocity; }
		inline float	GetWidth() const							{ return _width; }
		inline float	GetLength() const							{ return _length; }
		inline const CBone &	GetBone( EBone bone ) const			{ return _bones[bone]; }

		// Filtering helpers.
		CFinger			operator+( const CFinger &other ) const;
		CFinger			operator/( float scale ) const;
		CFinger			operator*( float scale ) const;

	private:
		int				_id;
		Vector			_direction;
		Vector			_tipPosition;
		Vector			_tipVelocity;
		float			_width, _length;
		CBone			_bones[BONE_COUNT];
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CHand
	{
	public:
						CHand();
#ifdef CLIENT_DLL
						CHand( const GiantLeap::Hand &h );
		void			FromLeap( const GiantLeap::Hand &h );
		void			BuildFingers( const GiantLeap::Hand &h );
#endif

		void			ToBitBuffer( bf_write *buf ) const;
		void			FromBitBuffer( bf_read *buf );

		void			Transform( float yaw, const Vector &translation );

		// Accessors.
		inline int		GetId() const								{ return _id; }
		inline float	GetConfidence() const						{ return _confidence; }
		inline float	GetPinchStrength() const					{ return _pinchStrength; }
		inline const Vector &	GetDirection() const				{ return _direction; }
		inline QAngle	GetAngles() const							{ QAngle angles; VectorAngles( _direction, angles ); return angles; }
		inline const Vector &	GetNormal() const					{ return _normal; }
		inline const Vector &	GetPosition() const					{ return _position; }
		inline const Vector &	GetVelocity() const					{ return _velocity; }
		inline const CFinger &	GetFingerByType( EFinger f ) const	{ return _fingers[f]; }
		inline const CArm &		GetArm() const						{ return _arm; }

		// Computed accessors.
		float			FindThetaBetweenFingers( EFinger f1, EFinger f2 ) const;
		inline const CFinger *	GetFingerById( int id ) const;
		inline const CFinger &	GetClosestFingerTo( EFinger to, EFinger f ) const;

		// Filtering helpers.
		CHand			operator+( const CHand &other ) const;
		CHand			operator/( float scale ) const;
		CHand			operator*( float scale ) const;

	private:
		int				_id;
		float			_confidence, _pinchStrength;
		Vector			_direction, _normal;
		Vector			_position;
		Vector			_velocity;
		CFinger			_fingers[EFinger::FINGER_COUNT];
		CArm			_arm;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CCircleGesture
	{
	public:
						CCircleGesture();
#ifdef CLIENT_DLL
						CCircleGesture( const GiantLeap::CircleGesture &c );
		void			FromLeap( const GiantLeap::CircleGesture &c );
#endif

		void			ToBitBuffer( bf_write *buf ) const;
		void			FromBitBuffer( bf_read *buf );

		void			Transform( float yaw, const Vector &translation );

		// Accessors.
		inline int		GetHandId() const							{ return _handId; }
		inline int		GetFingerId() const							{ return _fingerId; }
		inline float	GetRadius() const							{ return _radius; }
		inline float	GetDuration() const							{ return _duration; }
		inline const Vector &	GetCenter() const					{ return _center; }
		inline const Vector &	GetNormal() const					{ return _normal; }
		inline bool		IsClockwise() const							{ return _clockwise; }

		// Filtering helpers.
		CCircleGesture	operator+( const CCircleGesture &other ) const;
		CCircleGesture	operator/( float scale ) const;
		CCircleGesture	operator*( float scale ) const;

	private:
		int				_handId, _fingerId;
		float			_radius, _duration;
		Vector			_center, _normal;
		bool			_clockwise;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CSwipeGesture
	{
	public:
						CSwipeGesture();
#ifdef CLIENT_DLL
						CSwipeGesture( const GiantLeap::SwipeGesture &s );
		void			FromLeap( const GiantLeap::SwipeGesture &s );
#endif

		void			ToBitBuffer( bf_write *buf ) const;
		void			FromBitBuffer( bf_read *buf );

		void			Transform( float yaw, const Vector &translation );

		// Accessors.
		inline int		GetHandId() const							{ return _handId; }
		inline float	GetSpeed() const							{ return _speed; }
		inline const Vector &	GetDirection() const				{ return _direction; }
		inline const Vector &	GetCurrentPosition() const			{ return _curPosition; }
		inline const Vector &	GetStartPosition() const			{ return _startPosition; }

		// Filtering helpers.
		CSwipeGesture	operator+( const CSwipeGesture &other ) const;
		CSwipeGesture	operator/( float scale ) const;
		CSwipeGesture	operator*( float scale ) const;

	private:
		int				_handId;
		float			_speed;
		Vector			_direction, _curPosition, _startPosition;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CTapGesture
	{
	public:
						CTapGesture();
#ifdef CLIENT_DLL
						CTapGesture( const GiantLeap::KeyTapGesture &k );
						CTapGesture( const GiantLeap::ScreenTapGesture &s );
		void			FromLeap( const GiantLeap::KeyTapGesture &k );
		void			FromLeap( const GiantLeap::ScreenTapGesture &s );
#endif

		void			ToBitBuffer( bf_write *buf ) const;
		void			FromBitBuffer( bf_read *buf );

		void			Transform( float yaw, const Vector &translation );

		// Accessors.
		inline int		GetHandId() const							{ return _handId; }
		inline int		GetFingerId() const							{ return _fingerId; }
		inline const Vector &	GetDirection() const				{ return _direction; }
		inline const Vector &	GetPosition() const					{ return _position; }

		// Filtering helpers.
		CTapGesture		operator+( const CTapGesture &other ) const;
		CTapGesture		operator/( float scale ) const;
		CTapGesture		operator*( float scale ) const;

	private:
		int				_handId, _fingerId;
		Vector			_direction, _position;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CBallGesture
	{
	public:
						CBallGesture();
#ifdef CLIENT_DLL
						CBallGesture( const GiantLeap::Hand &h );
		void			FromLeap( const GiantLeap::Hand &h );
#endif

		void			ToBitBuffer( bf_write *buf ) const;
		void			FromBitBuffer( bf_read *buf );

		void			Transform( float yaw, const Vector &translation );

		// Accessors.
		inline int		GetHandId() const							{ return _handId; }
		inline float	GetRadius() const							{ return _radius; }
		inline float	GetGrabStrength() const						{ return _grabStrength; }
		inline Vector	GetCenter() const							{ return _center; }

		// Filtering helpers.
		CBallGesture	operator+( const CBallGesture &other ) const;
		CBallGesture	operator/( float scale ) const;
		CBallGesture	operator*( float scale ) const;

	private:
		int				_handId;
		float			_radius, _grabStrength;
		Vector			_center;
	};

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CFrame
	{
	public:
						CFrame();
#ifdef CLIENT_DLL
						CFrame( const GiantLeap::Frame &f );
		void			FromLeap( const GiantLeap::Frame &f );
#endif

		void			ToBitBuffer( bf_write *buf ) const;
		void			FromBitBuffer( bf_read *buf );

#ifdef GAME_DLL
		void			ToEntitySpace( CBaseCombatCharacter *entity, const Vector &delta );
#endif

		void			SetValid( bool valid )						{ _valid = valid; }
		bool			IsValid() const;

		bool			IsGestureActive( EGesture gesture ) const	{ return ( _gestureBits & ( 1 << gesture ) ) != 0; }
		void			SetGestureActive( EGesture gesture )		{ _gestureBits |= ( 1 << gesture ); }

		// Frame data accessors.
		inline const CHand &	GetHand() const						{ return _hand; }
		inline const CHand *	GetHandById( int id ) const;
		inline const CBallGesture &	GetBallGesture() const			{ return _ball; }
		inline const CCircleGesture &	GetCircleGesture() const	{ return _circle; }
		inline const CSwipeGesture &	GetSwipeGesture() const		{ return _swipe; }
		inline const CTapGesture &	GetTapGesture() const			{ return _tap; }

		// Frame data mutators.
		inline void		SetHand( const CHand &h )					{ _hand = h; }
		inline void		SetBallGesture( const CBallGesture &g )		{ _ball = g; }
		inline void		SetCircleGesture( const CCircleGesture &g )	{ _circle = g;	SetGestureActive( GESTURE_CIRCLE ); }
		inline void		SetSwipeGesture( const CSwipeGesture &g )	{ _swipe = g;	SetGestureActive( GESTURE_SWIPE ); }
		inline void		SetTapGesture( const CTapGesture &g )		{ _tap = g;		SetGestureActive( GESTURE_TAP ); }

		// Filtering helpers.
		CFrame			operator+( const CFrame &other ) const;
		CFrame			operator/( float scale ) const;
		CFrame			operator*( float scale ) const;

	private:
		template<class T> 
		T				AddGesture( const CFrame &other, const T &g1, const T &g2, EGesture g ) const;

		// Frame data.
		CHand			_hand;
		CBallGesture	_ball;
		CCircleGesture	_circle;
		CSwipeGesture	_swipe;
		CTapGesture		_tap;

		bool			_valid;
		int				_gestureBits;
	};
	
}

#endif // __HOLO_SHARED_H__