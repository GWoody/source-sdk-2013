/*
===============================================================================

	grid_player.cpp
	Implements functionality specific for the Grid player.

===============================================================================
*/

#include "cbase.h"
#include "grid_player.h"

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

	SendPropEHandle	( SENDINFO(m_hHand) ),

END_SEND_TABLE()

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
	CGunGesture gun = _gestureDetector.DetectGunGesture();
	if( gun.IsActive() )
	{
		
	}
}

//-----------------------------------------------------------------------------
// Called before the players update function.
//-----------------------------------------------------------------------------
void CGridPlayer::PreThink()
{
	BaseClass::PreThink();
}

//-----------------------------------------------------------------------------
// Called after the players update function.
//-----------------------------------------------------------------------------
void CGridPlayer::PostThink()
{
	BaseClass::PostThink();
}