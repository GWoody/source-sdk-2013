/*
===============================================================================

	grid_player.cpp
	Implements functionality specific for the Grid player.

===============================================================================
*/

#include "cbase.h"
#include "holo_player.h"
#include "holo_gesture_detector.h"
#include "holo_gesture_listener.h"
#include "holo_world_screen.h"

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

	DEFINE_FIELD( _hands, FIELD_EHANDLE ),

END_DATADESC()

// Networking table.
IMPLEMENT_SERVERCLASS_ST( CHoloPlayer, DT_HoloPlayer )

	SendPropArray3( SENDINFO_ARRAY3(_hands), SendPropEHandle( SENDINFO_ARRAY(_hands) ) ),
	SendPropVector( SENDINFO( _viewoffset ) ),
	
END_SEND_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CHoloPlayer::CHoloPlayer() : 
	_screenManager( this )
{
	_viewoffset.GetForModify().Init( 0, 0, 0 );
	_invalidFrames = 0;
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
		_hands.Set( i, pHand );
	}

	BaseClass::Spawn();

	SetNextThink( gpGlobals->curtime + 0.01f );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloPlayer::Think()
{
	BaseClass::Think();
	SetNextThink( gpGlobals->curtime + 0.01f );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloPlayer::Event_Killed( const CTakeDamageInfo &info )
{
	for( int i = 0; i < EHand::HAND_COUNT; i++ )
	{
		CHoloHand *hand = (CHoloHand *)_hands.Get( i ).Get();
		hand->OwnerKilled();
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
			CHoloHand *hand = (CHoloHand *)_hands[i].Get();
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
	//
	// Build the frame from all the usercmds.
	//
	CFrame finalHoloFrame = AccumulateHoloFrame( cmds, numcmds, totalcmds, dropped_packets, paused );

	//
	// Determine the source for the frame data processing.
	//
	CFrame *pFrame = NULL;
	if( !finalHoloFrame.IsValid() && _invalidFrames < 10 )
	{
		// Allow 10 invalid frames before rejecting processing.
		pFrame = &_lastValidFrame;
		_invalidFrames++;
	}
	else if( finalHoloFrame.IsValid() )
	{
		pFrame = &finalHoloFrame;
		_lastValidFrame = finalHoloFrame;
		_invalidFrames = 0;
	}

	//
	// Process the selected frame.
	//
	debugoverlay->ClearAllOverlays();
	if( pFrame )
	{
		for( int i = 0; i < EHand::HAND_COUNT; i++ )
		{
			CHoloHand *hand = (CHoloHand *)_hands[i].Get();
			hand->ProcessFrame( *pFrame );
		}

		ProcessFrame( *pFrame );
	}
	else
	{
		for( int i = 0; i < EHand::HAND_COUNT; i++ )
		{
			CHoloHand *hand = (CHoloHand *)_hands[i].Get();
			hand->OnInvalidFrame();
		}

		OnInvalidFrame();
	}

	//
	// Apply the headtracked offset.
	//
	const Vector eyeOffset = GetFlags() & FL_DUCKING ? VEC_DUCK_VIEW_SCALED( this ) : VEC_VIEW_SCALED( this );
	_viewoffset = cmds[totalcmds-1].viewoffset;
	SetViewOffset( eyeOffset + _viewoffset );

	BaseClass::ProcessUsercmds( cmds, numcmds, totalcmds, dropped_packets, paused );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloPlayer::LevelShutdown()
{
	//
	// Kill haptics.
	//
	for( int i = 0; i < HAND_COUNT; i++ )
	{
		GetHandEntity( (EHand)i )->LevelShutdown();
	}
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
		if( curframe.IsGestureActive( GESTURE_CIRCLE ) )
		{
			finalHoloFrame.SetCircleGesture( curframe.GetCircleGesture() );
		}
		if( curframe.IsGestureActive( GESTURE_SWIPE ) )
		{
			finalHoloFrame.SetSwipeGesture( curframe.GetSwipeGesture() );
		}
		if( curframe.IsGestureActive( GESTURE_TAP ) )
		{
			finalHoloFrame.SetTapGesture( curframe.GetTapGesture() );
		}

		finalHoloFrame.SetHand( curframe.GetHand( HAND_LEFT ), HAND_LEFT );
		finalHoloFrame.SetHand( curframe.GetHand( HAND_RIGHT ), HAND_RIGHT );

		finalHoloFrame.SetValid( true );
	}

	return finalHoloFrame;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloPlayer::ProcessFrame( const CFrame &frame )
{
	//
	// Detect custom gestures.
	//
	HandlePickupGesture( frame );
	HandleScreenGesture( frame );

	//
	// Handle map callbacks.
	//
	CHoloGestureListener *listener = CHoloGestureListener::Get();
	if( listener && frame.IsValid() )
	{
		if( frame.IsGestureActive( EGesture::GESTURE_CIRCLE ) )
		{
			listener->OnCircleGesture( frame, frame.GetCircleGesture() );
		}
		
		if( frame.IsGestureActive( EGesture::GESTURE_SWIPE ) )
		{
			listener->OnSwipeGesture( frame, frame.GetSwipeGesture() );
		}

		if( frame.IsGestureActive( EGesture::GESTURE_TAP ) )
		{
			listener->OnTapGesture( frame, frame.GetTapGesture() );
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloPlayer::OnInvalidFrame()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloPlayer::HandlePickupGesture( const CFrame &frame )
{
	if( !CanAttemptPickup() )
	{
		return;
	}

	for( int i = 0; i < EHand::HAND_COUNT; i++ )
	{
		CPickupGesture pickup( frame, (EHand)i );
		if( pickup.IsActive() )
		{
			CHoloHand *hand = (CHoloHand *)_hands[i].Get();
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
void CHoloPlayer::HandleScreenGesture( const CFrame &frame )
{
	if( _screenManager.IsScreenActive( WORLD_PANEL_MIDDLE ) )
	{
		return;
	}

	for( int i = 0; i < EHand::HAND_COUNT; i++ )
	{
		CScreenGesture pickup( frame, (EHand)i );
		if( pickup.IsActive() )
		{
			extern string_t global_screen_gesture_screen;
			_screenManager.CreateScreen( WORLD_PANEL_MIDDLE, STRING( global_screen_gesture_screen ) );
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float CHoloPlayer::GetHeldObjectMass( IPhysicsObject *pHeldObject )
{
	for( int i = 0; i < EHand::HAND_COUNT; i++ )
	{
		CHoloHand *hand = (CHoloHand *)_hands[i].Get();
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