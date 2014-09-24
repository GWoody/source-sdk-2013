/*
===============================================================================

	grid_player.cpp
	Implements functionality specific for the Grid player.

===============================================================================
*/

#include "cbase.h"
#include "grid_player.h"
#include "grid_base_weapon.h"

using namespace holo;
using namespace grid;

//-----------------------------------------------------------------------------
// Source entity configuration.
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( player, CGridPlayer );

// Define fields.
BEGIN_DATADESC( CGridPlayer )

	DEFINE_FIELD( m_hHand, FIELD_EHANDLE ),

END_DATADESC()

// Networking table.
IMPLEMENT_SERVERCLASS_ST( CGridPlayer, DT_GridPlayer )

	SendPropEHandle( SENDINFO( m_hHand ) ),
	SendPropEHandle( SENDINFO( _activeWeapon ) ),
	SendPropVector( SENDINFO( _viewoffset ) ),

END_SEND_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGridPlayer::CGridPlayer() : _inventory( this )
{
	_viewoffset.GetForModify().Init( 0, 0, 0 );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridPlayer::Spawn()
{
	SetModel( "models/player.mdl" );

	// Create the hand entity.
	CHoloHand *pHand = dynamic_cast<CHoloHand *>( CreateEntityByName( "holo_hand" ) );
	Assert( pHand );
	pHand->Spawn();
	pHand->SetOwnerEntity( this );
	m_hHand.Set( pHand );

	_weaponWasOut = false;

	// Enable all gestures.
	_gestureDetector.SetGestureEnabled( grid::EGesture::PICKUP, true );
	_gestureDetector.SetGestureEnabled( grid::EGesture::GUN, true );

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Vector CGridPlayer::Weapon_ShootPosition()
{
	return m_hHand->GetAbsOrigin();
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
	
	m_hHand->ProcessFrame( finalHoloFrame );
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

		finalHoloFrame.SetBallGesture( curframe.GetBallGesture() );
		finalHoloFrame.SetHand( curframe.GetHand() );
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
	CPickupGesture pickup = _gestureDetector.DetectPickupGesture();
	if( pickup.IsActive() )
	{
		if( pickup.HasClenchStarted() )
		{
			SetAttemptObjectPickup( true );
		}
		else if( pickup.HasClenchFinished() )
		{
			SetAttemptObjectPickup( false );
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

	CGunGesture gun = _gestureDetector.DetectGunGesture();
	if( gun.IsActive() )
	{
		if( !_weaponWasOut )
		{
			// The gun gesture was first made this frame. Show the gun in place of the hand.
			weapon->TakeOut();
			_activeWeapon = weapon;
			m_hHand->SetInvisible( true );
		}

		weapon->SetTriggerState( gun.HoldingTrigger() );

		_weaponWasOut = true;
	}
	else if( _weaponWasOut )
	{
		// All gun gestures have been stopped. Return the hand to the normal state.
		weapon->PutAway();
		_activeWeapon = NULL;
		m_hHand->SetInvisible( false );
		_weaponWasOut = false;
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
			const Vector &pointerDir = m_hHand->GetFrame().GetHand().GetFingerByType( holo::EFinger::FINGER_POINTER ).GetDirection();
			weapon->SetDirection( pointerDir );
			weapon->SetAbsOrigin( m_hHand->GetAbsOrigin() );
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