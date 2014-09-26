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

using namespace holo;
using namespace grid;

//-----------------------------------------------------------------------------
// Source entity configuration.
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( player, CGridPlayer );

// Define fields.
BEGIN_DATADESC( CGridPlayer )

	DEFINE_FIELD( m_hHand, FIELD_EHANDLE ),
	DEFINE_EMBEDDED( _haptics ),

END_DATADESC()

// Networking table.
IMPLEMENT_SERVERCLASS_ST( CGridPlayer, DT_GridPlayer )

	SendPropArray3( SENDINFO_ARRAY3(m_hHand), SendPropEHandle( SENDINFO_ARRAY(m_hHand) ) ),
	SendPropEHandle( SENDINFO( _activeWeapon ) ),
	SendPropVector( SENDINFO( _viewoffset ) ),
	SendPropDataTable( SENDINFO_DT(_haptics), &REFERENCE_SEND_TABLE(DT_HoloHaptics), SendProxy_SendLocalDataTable ),
	
END_SEND_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGridPlayer::CGridPlayer() : _inventory( this )
{
	_viewoffset.GetForModify().Init( 0, 0, 0 );
	_weaponHandIdx = -1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridPlayer::Spawn()
{
	SetModel( "models/player.mdl" );

	// Create the hand entity.
	for( int i = 0; i < EHand::HAND_COUNT; i++ )
	{
		CHoloHand *pHand = dynamic_cast<CHoloHand *>( CreateEntityByName( "holo_hand" ) );
		Assert( pHand );
		pHand->Spawn();
		pHand->SetOwnerEntity( this ); 
		pHand->SetType( (EHand)i );
		m_hHand.Set( i, pHand );
	}

	_weaponWasOut = false;

	// Enable all gestures.
	_gestureDetector.SetGestureEnabled( grid::EGesture::PICKUP, true );
	_gestureDetector.SetGestureEnabled( grid::EGesture::GUN, true );

	BaseClass::Spawn();

	SetNextThink( gpGlobals->curtime + 0.01f );

	_haptics.SetEnabled( true );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridPlayer::Think()
{
	_haptics.Update();
	NetworkStateChanged();

	BaseClass::Think();

	SetNextThink( gpGlobals->curtime + 0.01f );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridPlayer::Event_Killed( const CTakeDamageInfo &info )
{
	_haptics.ClearAllEvents();

	for( int i = 0; i < EHand::HAND_COUNT; i++ )
	{
		CBaseHoloHand *hand = (CBaseHoloHand *)m_hHand.Get( i ).Get();
		hand->ClearUseEntity();
	}

	BaseClass::Event_Killed( info );
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

	return m_hHand[_weaponHandIdx]->GetAbsOrigin();
}

//-----------------------------------------------------------------------------
// Called whenever a player collides with a weapon entity.
//-----------------------------------------------------------------------------
bool CGridPlayer::BumpWeapon( CBaseCombatWeapon *pWeapon )
{
	// We don't pickup weapons by colliding with them.
	return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridPlayer::ProcessUsercmds( CUserCmd *cmds, int numcmds, int totalcmds, int dropped_packets, bool paused )
{
	CFrame finalHoloFrame = AccumulateHoloFrame( cmds, numcmds, totalcmds, dropped_packets, paused );
	
	for( int i = 0; i < EHand::HAND_COUNT; i++ )
	{
		CHoloHand *hand = (CHoloHand *)m_hHand[i].Get();
		hand->ProcessFrame( finalHoloFrame );
	}

	if( finalHoloFrame.IsValid() )
	{
		ProcessFrame( finalHoloFrame );
	}

	const Vector eyeOffset = GetFlags() & FL_DUCKING ? VEC_DUCK_VIEW_SCALED( this ) : VEC_VIEW_SCALED( this );
	_viewoffset = cmds[totalcmds-1].viewoffset;
	SetViewOffset( eyeOffset + _viewoffset );

	BaseClass::ProcessUsercmds( cmds, numcmds, totalcmds, dropped_packets, paused );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CFrame CGridPlayer::AccumulateHoloFrame( CUserCmd *cmds, int numcmds, int totalcmds, int dropped_packets, bool paused )
{
	CFrame finalHoloFrame;
	finalHoloFrame.SetValid( false );

	for ( int i = totalcmds - 1; i >= 0; i-- )
	{
		CUserCmd *pCmd = &cmds[totalcmds - 1 - i];

		// Validate values
		if ( !IsUserCmdDataValid( pCmd ) )
		{
			pCmd->MakeInert();
			continue;
		}

		// Take the newest version of all frame attributes.
		const CFrame &curframe = pCmd->holo_frame;
		if( curframe.IsGestureActive( holo::EGesture::GESTURE_CIRCLE ) )
		{
			finalHoloFrame.SetCircleGesture( curframe.GetCircleGesture() );
		}
		if( curframe.IsGestureActive( holo::EGesture::GESTURE_SWIPE ) )
		{
			finalHoloFrame.SetSwipeGesture( curframe.GetSwipeGesture() );
		}
		if( curframe.IsGestureActive( holo::EGesture::GESTURE_TAP ) )
		{
			finalHoloFrame.SetTapGesture( curframe.GetTapGesture() );
		}

		finalHoloFrame.SetHand( curframe.GetHand( EHand::LEFT ), EHand::LEFT );
		finalHoloFrame.SetHand( curframe.GetHand( EHand::RIGHT ), EHand::RIGHT );

		finalHoloFrame.SetValid( true );
	}

	return finalHoloFrame;
}

//-----------------------------------------------------------------------------
// Detects custom user gestures.
//-----------------------------------------------------------------------------
void CGridPlayer::ProcessFrame( const holo::CFrame &frame )
{
	_gestureDetector.SetFrame( frame );

	HandlePickupGesture();
	HandleGunGesture();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridPlayer::HandlePickupGesture()
{
	for( int i = 0; i < EHand::HAND_COUNT; i++ )
	{
		CPickupGesture pickup = _gestureDetector.DetectPickupGesture( (EHand)i );
		if( pickup.IsActive() )
		{
			CHoloHand *hand = (CHoloHand *)m_hHand[i].Get();
			if( pickup.HasClenchStarted() )
			{
				hand->AttemptObjectPickup();
			}
			else if( pickup.HasClenchFinished() )
			{
				hand->AttemptObjectDrop();
			}
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridPlayer::HandleGunGesture()
{
	CGridBaseWeapon *weapon = GetInventory().GetWeapon();
	if( !weapon )
	{
		return;
	}

	for( int i = 0; i < EHand::HAND_COUNT; i++ )
	{
		CGunGesture gun = _gestureDetector.DetectGunGesture( (EHand)i );
		CHoloHand *hand = (CHoloHand *)m_hHand[i].Get();
		if( gun.IsActive() )
		{
			if( !_weaponWasOut )
			{
				// The gun gesture was first made this frame. Show the gun in place of the hand.
				_activeWeapon = weapon;
				_weaponHandIdx = i;
				_activeWeapon->TakeOut();
				hand->SetInvisible( true );
			}

			weapon->SetTriggerState( gun.HoldingTrigger() );

			_weaponWasOut = true;
			_weaponHandIdx = i;
			break;
		}
		else if( i == _weaponHandIdx && _weaponWasOut && !gun.IsActive() )
		{
			// All gun gestures have been stopped. Return the hand to the normal state.
			weapon->PutAway();
			hand->SetInvisible( false );

			_activeWeapon = NULL;
			_weaponWasOut = false;
			_weaponHandIdx = -1;
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

	if( CGridBaseWeapon *weapon = _inventory.GetWeapon() )
	{
		if( weapon->IsOut() )
		{
			CHoloHand *hand = (CHoloHand *)m_hHand[_weaponHandIdx].Get();
			const Vector &pointerDir = hand->GetHoloHand().GetFingerByType( holo::EFinger::FINGER_POINTER ).GetDirection();
			weapon->SetDirection( pointerDir );
			weapon->SetAbsOrigin( hand->GetAbsOrigin() );
			weapon->ItemPreFrame();
		}
	}
}

//-----------------------------------------------------------------------------
// Called after the players update function.
//-----------------------------------------------------------------------------
void CGridPlayer::PostThink()
{
	BaseClass::PostThink();
}