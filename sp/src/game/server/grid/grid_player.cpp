/*
===============================================================================

	grid_player.cpp
	Implements functionality specific for the Grid player.

===============================================================================
*/

#include "cbase.h"
#include "grid_player.h"
#include "grid_base_weapon.h"
#include "holodeck/holo_haptics.h"
#include "grid/grid_haptic_events.h"
#include "grid_proptool.h"

using namespace holo;
using namespace grid;

//-----------------------------------------------------------------------------
// Source entity configuration.
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( player, CGridPlayer );

// Define fields.
BEGIN_DATADESC( CGridPlayer )
END_DATADESC()

// Networking table.
IMPLEMENT_SERVERCLASS_ST( CGridPlayer, DT_GridPlayer )

	SendPropEHandle( SENDINFO( _activeWeapon ) ),
	
END_SEND_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGridPlayer::CGridPlayer() : _inventory( this )
{
	_weaponHandIdx = -1;
	ListenForGameEvent( "grid_ready_proptool" );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridPlayer::FireGameEvent( IGameEvent *event )
{
	if( !Q_strcmp( event->GetName(), "grid_ready_proptool" ) )
	{
		const char *prop = event->GetString( "prop" );
		
		CGridPropTool *tool = (CGridPropTool *)CreateEntityByName( "grid_proptool" );
		tool->SetOwnerEntity( this );
		tool->SetProp( prop );
		tool->TakeOut( HAND_RIGHT );

		_inventory.RemoveWeapon();
		_inventory.SwapWeapons( tool );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridPlayer::Spawn()
{
	_weaponHandIdx = -1;
	_weaponWasOut = false;

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CGridPlayer::IsUseableEntity( CBaseEntity *pEntity, unsigned int requiredCaps )
{
	if( pEntity )
	{
		// Can't use the weapon we own.
		if( _inventory.GetWeapon() && pEntity->entindex() == _inventory.GetWeapon()->entindex() )
		{
			return false;
		}
	}

	return BaseClass::IsUseableEntity( pEntity, requiredCaps );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Vector CGridPlayer::Weapon_ShootPosition()
{
	if( _weaponHandIdx == -1 )
	{
		Warning( "Calling "__FUNCTION__" without holding a weapon!\n" );
		return Vector();
	}

	return GetHandEntity( (EHand)_weaponHandIdx )->GetAbsOrigin();
}

//-----------------------------------------------------------------------------
// Detects custom user gestures.
//-----------------------------------------------------------------------------
void CGridPlayer::ProcessFrame( const CFrame &frame )
{
	HandleGunGesture( frame );
	BaseClass::ProcessFrame( frame );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridPlayer::OnInvalidFrame()
{
	//
	// Disable guns.
	//
	if( _activeWeapon && !_activeWeapon->IsPropTool() )
	{
		CHoloHand *hand = (CHoloHand *)GetHandEntity( (EHand)_weaponHandIdx );

		_activeWeapon->PutAway();
		hand->SetInvisible( false );

		_activeWeapon = NULL;
		_weaponWasOut = false;
		_weaponHandIdx = -1;
	}

	BaseClass::OnInvalidFrame();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridPlayer::HandleGunGesture( const CFrame &frame )
{
	CGridBaseWeapon *weapon = GetInventory().GetWeapon();
	if( !weapon || GetUseEntity() )
	{
		return;
	}

	for( int i = 0; i < EHand::HAND_COUNT; i++ )
	{
		CGunGesture gun(frame, (EHand)i );
		CHoloHand *hand = (CHoloHand *)GetHandEntity( (EHand)i );
		if( gun.IsActive() )
		{
			if( !_weaponWasOut )
			{
				// The gun gesture was first made this frame. Show the gun in place of the hand.
				_activeWeapon = weapon;
				_weaponHandIdx = i;
				_activeWeapon->TakeOut( (EHand)i );

				if( !weapon->IsPropTool() )
				{
					hand->SetInvisible( true );
				}
			}

			weapon->SetTriggerState( gun.HoldingTrigger() );

			_weaponWasOut = true;
			_weaponHandIdx = i;
			break;
		}
		else if( i == _weaponHandIdx && _weaponWasOut && !gun.IsActive() )
		{
			if( !weapon->IsPropTool() )
			{
				// All gun gestures have been stopped. Return the hand to the normal state.
				weapon->PutAway();
				hand->SetInvisible( false );

				_activeWeapon = NULL;
				_weaponWasOut = false;
				_weaponHandIdx = -1;
			}
			
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Called before the players update function.
//-----------------------------------------------------------------------------
void CGridPlayer::PreThink()
{
	BaseClass::PreThink();

	CGridBaseWeapon *weapon = _inventory.GetWeapon();
	if( weapon && weapon->IsPropTool() )
	{
		weapon->ItemPreFrame();
	}
	else if( weapon )
	{
		weapon->SetAbsOrigin( EyePosition() );

		if( weapon->IsOut() && _weaponHandIdx != -1 )
		{
			CHoloHand *hand = (CHoloHand *)GetHandEntity( (EHand)_weaponHandIdx );
			weapon->SetAbsOrigin( hand->GetAbsOrigin() );

			const Vector &pointerDir = hand->GetHoloHand().GetFingerByType( ::FINGER_POINTER ).GetDirection();
			weapon->SetDirection( pointerDir );
			weapon->ItemPreFrame();
		}
	}

	if( _activeWeapon && !weapon )
	{
		_activeWeapon = NULL;
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int CGridPlayer::OnTakeDamage( const CTakeDamageInfo &info )
{
	if( info.GetDamageType() & DMG_SHOCK )
	{
		GetHandEntity( HAND_LEFT )->GetHaptics().AddEvent( new CShockDmgHapticEvent );
		GetHandEntity( HAND_RIGHT )->GetHaptics().AddEvent( new CShockDmgHapticEvent );
	}

	if( info.GetDamageType() & DMG_BULLET )
	{
		GetHandEntity( HAND_LEFT )->GetHaptics().AddEvent( new CBulletDmgHapticEvent );
		GetHandEntity( HAND_RIGHT )->GetHaptics().AddEvent( new CBulletDmgHapticEvent );
	}

	return BaseClass::OnTakeDamage( info );
}