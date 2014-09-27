/*
===============================================================================

	grid_player.cpp
	Implements functionality specific for the Grid player.

===============================================================================
*/

#include "cbase.h"
#include "holo_player.h"
#include "holo_gesture_detector.h"

using namespace holo;

//-----------------------------------------------------------------------------
// External functions.
//-----------------------------------------------------------------------------
extern float PlayerPickupGetHeldObjectMass( CBaseEntity *pPickupControllerEntity, IPhysicsObject *pHeldObject );

//-----------------------------------------------------------------------------
// Source entity configuration.
//-----------------------------------------------------------------------------

// Define fields.
BEGIN_DATADESC( CHoloPlayer )

	DEFINE_FIELD( m_hHand, FIELD_EHANDLE ),
	DEFINE_EMBEDDED( _haptics ),

END_DATADESC()

// Networking table.
IMPLEMENT_SERVERCLASS_ST( CHoloPlayer, DT_HoloPlayer )

	SendPropArray3( SENDINFO_ARRAY3(m_hHand), SendPropEHandle( SENDINFO_ARRAY(m_hHand) ) ),
	SendPropVector( SENDINFO( _viewoffset ) ),
	SendPropDataTable( SENDINFO_DT(_haptics), &REFERENCE_SEND_TABLE(DT_HoloHaptics), SendProxy_SendLocalDataTable ),
	
END_SEND_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CHoloPlayer::CHoloPlayer() : _haptics( 1 )
{
	_viewoffset.GetForModify().Init( 0, 0, 0 );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloPlayer::Spawn()
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

	BaseClass::Spawn();

	SetNextThink( gpGlobals->curtime + 0.01f );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloPlayer::Think()
{
	_haptics.Update();
	NetworkStateChanged();

	BaseClass::Think();
	SetNextThink( gpGlobals->curtime + 0.01f );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloPlayer::Event_Killed( const CTakeDamageInfo &info )
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
bool CHoloPlayer::IsUseableEntity( CBaseEntity *pEntity, unsigned int requiredCaps )
{
	if( pEntity )
	{
		for( int i = 0; i < EHand::HAND_COUNT; i++ )
		{
			// Can't use hands or the object the hand is holding.
			CHoloHand *hand = (CHoloHand *)m_hHand[i].Get();
			if( pEntity->entindex() == hand->entindex() )
			{
				return false;
			}

			if( hand->GetUseEntity() && pEntity->entindex() == hand->GetUseEntity()->entindex() )
			{
				return false;
			}
		}
	}

	return BaseClass::IsUseableEntity( pEntity, requiredCaps );
}

//-----------------------------------------------------------------------------
// Called whenever a player collides with a weapon entity.
//-----------------------------------------------------------------------------
bool CHoloPlayer::BumpWeapon( CBaseCombatWeapon *pWeapon )
{
	// We don't pickup weapons by colliding with them.
	return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloPlayer::ProcessUsercmds( CUserCmd *cmds, int numcmds, int totalcmds, int dropped_packets, bool paused )
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
CFrame CHoloPlayer::AccumulateHoloFrame( CUserCmd *cmds, int numcmds, int totalcmds, int dropped_packets, bool paused )
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
//-----------------------------------------------------------------------------
void CHoloPlayer::ProcessFrame( const holo::CFrame &frame )
{
	HandlePickupGesture( frame );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloPlayer::HandlePickupGesture( const holo::CFrame &frame )
{
	for( int i = 0; i < EHand::HAND_COUNT; i++ )
	{
		CPickupGesture pickup( frame, (EHand)i );
		if( pickup.IsActive() )
		{
			CHoloHand *hand = (CHoloHand *)m_hHand[i].Get();
			if( pickup.IsHandClenched() && !hand->GetUseEntity() )
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
float CHoloPlayer::GetHeldObjectMass( IPhysicsObject *pHeldObject )
{
	for( int i = 0; i < EHand::HAND_COUNT; i++ )
	{
		CHoloHand *hand = (CHoloHand *)m_hHand[i].Get();
		float mass = PlayerPickupGetHeldObjectMass( hand->GetUseEntity(), pHeldObject );
		if( mass != 0.0f )
		{
			return mass;
		}
	}

	return PlayerPickupGetHeldObjectMass( m_hUseEntity, pHeldObject );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloPlayer::PlayerUse ( void )
{
	// Nothing here.
	// Don't chain to base.
}