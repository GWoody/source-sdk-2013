/*
===============================================================================

	base_grabcontroller.h
		Mostly borrowed from `hl2/weapon_physcannon.cpp`

===============================================================================
*/

#ifndef __GRID_GRABCONTROLLER_H__
#define __GRID_GRABCONTROLLER_H__

class CBaseHoloHand;

//-----------------------------------------------------------------------------
// Derive from `hlshadowcontrol_params_t` so we can add save/load data to it
//-----------------------------------------------------------------------------
struct game_shadowcontrol_params_t : public hlshadowcontrol_params_t
{
	DECLARE_SIMPLE_DATADESC();
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGrabController : public IMotionEvent
{
	DECLARE_SIMPLE_DATADESC();

public:

	CGrabController( void );
	~CGrabController( void );
	void AttachEntity( CBaseHoloHand *pHand, CBaseEntity *pEntity, IPhysicsObject *pPhys, bool bIsMegaPhysCannon, const Vector &vGrabPosition, bool bUseGrabPosition );
	void DetachEntity( bool bClearVelocity );
	void OnRestore();

	bool UpdateObject( CBaseHoloHand *pHand, float flError );

	void SetTargetPosition( const Vector &target, const QAngle &targetOrientation );
	float ComputeError();
	float GetLoadWeight( void ) const { return m_flLoadWeight; }
	void SetAngleAlignment( float alignAngleCosine ) { m_angleAlignment = alignAngleCosine; }
	void SetIgnorePitch( bool bIgnore ) { m_bIgnoreRelativePitch = bIgnore; }
	QAngle TransformAnglesToPlayerSpace( const QAngle &anglesIn, CBasePlayer *pPlayer );
	QAngle TransformAnglesFromPlayerSpace( const QAngle &anglesIn, CBasePlayer *pPlayer );

	CBaseEntity *GetAttached() { return (CBaseEntity *)m_attachedEntity; }

	IMotionEvent::simresult_e Simulate( IPhysicsMotionController *pController, IPhysicsObject *pObject, float deltaTime, Vector &linear, AngularImpulse &angular );
	float GetSavedMass( IPhysicsObject *pObject );

	bool IsObjectAllowedOverhead( CBaseEntity *pEntity );

private:
	// Compute the max speed for an attached object
	void ComputeMaxSpeed( CBaseEntity *pEntity, IPhysicsObject *pPhysics );
		
	bool InContactWithHeavyObject( IPhysicsObject *pObject, float heavyMass );
	void ClampPhysicsVelocity( IPhysicsObject *pPhys, float linearLimit, float angularLimit );
	IPhysicsObject *GetRagdollChildAtPosition( CBaseEntity *pTarget, const Vector &position );

	QAngle AlignAngles( const QAngle &angles, float cosineAlignAngle );
	void MatrixOrthogonalize( matrix3x4_t &matrix, int column );
	void ComputePlayerMatrix( CBasePlayer *pPlayer, matrix3x4_t &out );

	game_shadowcontrol_params_t	m_shadow;
	float			m_timeToArrive;
	float			m_errorTime;
	float			m_error;
	float			m_contactAmount;
	float			m_angleAlignment;
	bool			m_bCarriedEntityBlocksLOS;
	bool			m_bIgnoreRelativePitch;

	float			m_flLoadWeight;
	float			m_savedRotDamping[VPHYSICS_MAX_OBJECT_LIST_COUNT];
	float			m_savedMass[VPHYSICS_MAX_OBJECT_LIST_COUNT];
	EHANDLE			m_attachedEntity;
	QAngle			m_vecPreferredCarryAngles;
	bool			m_bHasPreferredCarryAngles;
	float			m_flDistanceOffset;

	QAngle			m_attachedAnglesPlayerSpace;
	Vector			m_attachedPositionObjectSpace;

	IPhysicsMotionController *m_controller;

	bool			m_bAllowObjectOverhead; // Can the player hold this object directly overhead? (Default is NO)

	// NVNT player controlling this grab controller
	CBaseHoloHand*	m_pControllingHand;

	friend class CWeaponPhysCannon;
};

#endif // __GRID_GRABCONTROLLER_H__