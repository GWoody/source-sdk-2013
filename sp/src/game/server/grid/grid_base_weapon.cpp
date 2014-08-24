/*
===============================================================================

	grid_base_weapon.cpp
	Implements functionality core to all grid weapons.

===============================================================================
*/

#include "cbase.h"
#include "grid_player.h"
#include "grid_base_weapon.h"
#include "particle_parse.h"

using namespace grid;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGridBaseWeapon::CGridBaseWeapon( const char *script )
{
	_info.Parse( script );

	// Init state.
	_remainingShots = _info.GetBullet().GetCount();
	_triggerHeld = false;
	_nextFireTime = 0.0f;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGridBaseWeapon::~CGridBaseWeapon()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::Precache()
{
	PrecacheModel( _info.GetModel().GetWeapon() );
	PrecacheModel( _info.GetModel().GetShell() );

	PrecacheScriptSound( _info.GetSound().GetEmpty() );
	PrecacheScriptSound( _info.GetSound().GetFire() );

	if( const char *particle = _info.GetEffect().GetMuzzleParticleName() )
	{
		PrecacheParticleSystem( particle );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::Spawn()
{
	Precache();

	SetModel( _info.GetModel().GetWeapon() );
	SetSolid( SOLID_BBOX );
	SetCollisionGroup( COLLISION_GROUP_DEBRIS );
	SetBlocksLOS( false );

	// Make the object physically simulated.
	VPhysicsInitNormal( GetSolid(), GetSolidFlags(), false );

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value )
{
	CGridPlayer *player = dynamic_cast<CGridPlayer *>( activator );
	if( !player )
	{
		return;
	}

	player->GetInventory().SwapWeapons( this );
	player->SetAttemptObjectPickup( false );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int CGridBaseWeapon::ObjectCaps()
{
	return BaseClass::ObjectCaps() | FCAP_IMPULSE_USE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::SwapWith( CGridBaseWeapon *weapon )
{
	SetMoveType( MOVETYPE_VPHYSICS );
	SetOwnerEntity( NULL );
	RemoveEffects( EF_NODRAW );

	SetAbsOrigin( weapon->GetAbsOrigin() );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::Pickup( CGridPlayer *player )
{
	SetMoveType( MOVETYPE_NONE );
	SetAbsVelocity( vec3_origin );
	SetOwnerEntity( player );
	AddEffects( EF_NODRAW );
	VPhysicsDestroyObject();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Vector VecCheckToss ( CBaseEntity *pEdict, Vector vecSpot1, Vector vecSpot2, float flHeightMaxRatio, float flGravityAdj, bool bRandomize, Vector *vecMins = NULL, Vector *vecMaxs = NULL );
void CGridBaseWeapon::Drop( const Vector &target )
{
	CGridPlayer *player = dynamic_cast<CGridPlayer *>( GetOwnerEntity() );
	Assert( player );

	VPhysicsInitNormal( GetSolid(), GetSolidFlags(), false );

	SetMoveType( MOVETYPE_VPHYSICS );
	SetOwnerEntity( NULL );
	SetAbsVelocity( player->EyeDirection2D() * 1000.0f );
	RemoveEffects( EF_NODRAW );

	// Launch the weapon in the direction the player is looking.
	Vector throwDir = target - GetAbsOrigin();
	throwDir *= 3.5f;
	IPhysicsObject *pObj = VPhysicsGetObject();
	if ( pObj != NULL )
	{
		AngularImpulse	angImp( 200, 200, 200 );
		pObj->AddVelocity( &throwDir, &angImp );
	}
	else
	{
		SetAbsVelocity( throwDir );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::TakeOut()
{
	RemoveEffects( EF_NODRAW );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::PutAway()
{
	SetTriggerState( false );
	AddEffects( EF_NODRAW );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::SetTriggerState( bool pressed )
{
	if( pressed != _triggerHeld )
	{
		_firedSinceTrigger = false;
	}

	_triggerHeld = pressed;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::SetDirection( const Vector &dir )
{
	_direction = dir;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::ItemPreFrame()
{
	CommitAngle();

	if( _triggerHeld )
	{
		Shoot();
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::PlayShootSound()
{
	EmitSound( _info.GetSound().GetFire() );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::PlayEmptySound()
{
	EmitSound( _info.GetSound().GetEmpty() );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::ShootSingleBullet()
{
	if( !_remainingShots )
	{
		PlayEmptySound();
		return;
	}

	DoMuzzleFlash();
	PerformImpactTrace();
	PlayShootSound();	

	_remainingShots--;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::DoMuzzleFlash()
{
	const CEffectInfo &effect = _info.GetEffect();

	const char *particle = effect.GetMuzzleParticleName();
	int attachment = effect.GetMuzzleAttachment();
	if( particle && attachment != -1 )
	{
		Vector origin;
		QAngle angles;
		GetAttachment( attachment, origin, angles );

		angles = GetAbsAngles() - QAngle( 180, 0, 0 );

		DispatchParticleEffect( particle, origin, angles );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::PerformImpactTrace()
{
	int attachment = _info.GetEffect().GetMuzzleAttachment();
	if( attachment == -1 )
	{
		Warning( "CGridBaseWeapon::PerformImpactTrace - missing muzzle attachment!!!\n" );
		return;
	}

	// Start from the weapon muzzle position.
	Vector start;
	QAngle angles;
	GetAttachment( attachment, start, angles );

	const CBulletInfo &bullet = _info.GetBullet();
	FireBulletsInfo_t info;

	info.m_flDamage = bullet.GetDamage();
	info.m_flDistance = bullet.GetMaxDistance();
	info.m_iAmmoType = bullet.GetAmmoType();
	info.m_vecDirShooting = _direction;
	info.m_vecSrc = start;
	info.m_pAdditionalIgnoreEnt = GetOwnerEntity();

	FireBullets( info );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::MakeTracer( const Vector &start, const Vector &end )
{
	// Temp.
	debugoverlay->AddLineOverlay( start, end, 240, 230, 180, false, gpGlobals->frametime );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::Shoot()
{
	const CShootInfo &shoot = _info.GetShoot();
	if( _firedSinceTrigger && shoot.IsSemiAuto() )
	{
		// Wait for the trigger to be released in semi auto mode.
		return;
	}

	if( _nextFireTime <= gpGlobals->curtime )
	{
		if( !_remainingShots )
		{
			PlayEmptySound();
		}
		else
		{
			ShootSingleBullet();
		}

		_nextFireTime = gpGlobals->curtime + shoot.GetRate();
	}

	_firedSinceTrigger = true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::CommitAngle()
{
	QAngle angles;
	VectorAngles( _direction, angles );

	angles += _info.GetModel().GetAngle();
	angles.x = -angles.x;

	SetAbsAngles( angles );
}