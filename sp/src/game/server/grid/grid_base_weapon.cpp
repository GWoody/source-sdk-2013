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
#include "te_effect_dispatch.h"

using namespace grid;

//-----------------------------------------------------------------------------
// Source Networking.
//-----------------------------------------------------------------------------
BEGIN_DATADESC( CGridBaseWeapon )

	DEFINE_FIELD( _triggerHeld, FIELD_BOOLEAN ),
	DEFINE_FIELD( _remainingShots, FIELD_INTEGER ),
	DEFINE_FIELD( _nextFireTime, FIELD_FLOAT ),
	DEFINE_FIELD( _direction, FIELD_VECTOR ),
	DEFINE_FIELD( _firedSinceTrigger, FIELD_BOOLEAN ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CGridBaseWeapon, DT_GridBaseWeapon )

	SendPropBool( SENDINFO( _triggerHeld ) ),
	SendPropInt( SENDINFO( _remainingShots ) ),

END_SEND_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGridBaseWeapon::CGridBaseWeapon( const char *script )
{
	_info.Parse( script );

	// Init state.
	_remainingShots = _info.GetBullet().GetCount();
	_triggerHeld = false;
	_nextFireTime = 0.0f;
	_ammoScreen.Set( NULL );
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
	PrecacheModel( _info.GetModel().GetWorldModel() );
	PrecacheModel( _info.GetModel().GetPlayerModel() );

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

	BaseClass::Spawn();

	SetModel( _info.GetModel().GetWorldModel() );
	SetSolid( SOLID_BBOX );
	SetCollisionGroup( COLLISION_GROUP_DEBRIS );
	SetBlocksLOS( false );

	// Make the object physically simulated.
	VPhysicsInitNormal( GetSolid(), GetSolidFlags() | FSOLID_TRIGGER, false );

	// Bloat the box for player pickup
	CollisionProp()->UseTriggerBounds( true, 36 );

	CreateAmmoScreen();
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
void CGridBaseWeapon::SetModel( const char *model )
{
	BaseClass::SetModel( model );
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
	SetModel( _info.GetModel().GetPlayerModel() );
	SetMoveType( MOVETYPE_NONE );
	SetAbsVelocity( vec3_origin );
	SetOwnerEntity( player );
	AddEffects( EF_NODRAW );
	VPhysicsDestroyObject();

	CreateAmmoScreen();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::Drop( const Vector &target )
{
	CGridPlayer *player = dynamic_cast<CGridPlayer *>( GetOwnerEntity() );
	Assert( player );

	SetModel( _info.GetModel().GetWorldModel() );

	VPhysicsInitNormal( GetSolid(), GetSolidFlags(), false );

	SetMoveType( MOVETYPE_VPHYSICS );
	SetOwnerEntity( NULL );
	RemoveEffects( EF_NODRAW );

	// Launch the weapon in the direction the player is looking.
	Vector throwDir = player->EyeDirection2D() * 350.0f;
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

	// TODO: destroy ammo screen.
	// TODO: LASER SIGHT
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
	EjectShell();

	_remainingShots--;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::DoMuzzleFlash()
{
	const CEffectInfo &effect = _info.GetEffect();

	const char *particle = effect.GetMuzzleParticleName();
	const char *attachmentname = effect.GetMuzzleAttachment();
	int attachment = LookupAttachment( attachmentname );
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
	const char *attachmentname = _info.GetEffect().GetMuzzleAttachment();
	int attachment = LookupAttachment( attachmentname );
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
void CGridBaseWeapon::EjectShell()
{
	int type = _info.GetEffect().GetShellType();
	if( type == -1 )
	{
		return;
	}

	const char *attachmentname = _info.GetEffect().GetShellAttachment();
	int attachment = LookupAttachment( attachmentname );
	if( attachment == -1 )
	{
		return;
	}

	// Get the direction and origin of the shell effect.
	Vector origin;
	QAngle angles;
	GetAttachment( attachment, origin, angles );

	type = clamp( type, 0, 2 );
	const char *typenames[] = { "ShellEject", "RifleShellEject", "ShotgunShellEject" };

	CEffectData data;
	data.m_vOrigin = origin;
	data.m_vAngles = angles;
	DispatchEffect( typenames[type], data );
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
void CGridBaseWeapon::CreateAmmoScreen()
{
	int attachment = LookupAttachment( _info.GetHud( ).GetAmmoAttachment() );
	if( attachment != -1 )
	{
		CVGuiScreen *screen = CreateVGuiScreen( "vgui_screen", "grid_ammo_screen", this, this, attachment );

		screen->SetActualSize( 2, 2 );
		screen->SetActive( true );
		screen->MakeVisibleOnlyToTeammates( false );
		screen->SetAttachedToViewModel( false );

		_ammoScreen.Set( screen );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridBaseWeapon::CommitAngle()
{
	QAngle angles;
	VectorAngles( _direction, angles );

	angles += _info.GetModel().GetAngle();

	// HL2 CONTENT HACK: Some weapons face different directions.
	// Weapons facing towards the player need their pitch rotation inverted.
	angles.x = angles.x * Sign( cos( _info.GetModel().GetAngle().y ) );

	SetAbsAngles( angles );
}