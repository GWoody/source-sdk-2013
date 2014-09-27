/*
===============================================================================

	base_pickupcontroller.cpp
		Mostly borrowed from `hl2/weapon_physcannon.cpp`

===============================================================================
*/

#include "cbase.h"
#include "base_pickupcontroller.h"
#include "player_pickup.h"
#include "in_buttons.h"
#include "physics_saverestore.h"
#include "grid_player.h"

//-----------------------------------------------------------------------------
// ConVars
//-----------------------------------------------------------------------------
ConVar player_throwforce( "player_throwforce", "100" );
ConVar physcannon_tracelength( "physcannon_tracelength", "250" );

LINK_ENTITY_TO_CLASS( player_pickup, CPlayerPickupController );

//---------------------------------------------------------
// Save/Restore
//---------------------------------------------------------
BEGIN_DATADESC( CPlayerPickupController )

	DEFINE_EMBEDDED( m_grabController ),

	// Physptrs can't be inside embedded classes
	DEFINE_PHYSPTR( m_grabController.m_controller ),

	DEFINE_FIELD( m_pHand,		FIELD_CLASSPTR ),
	
END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pPlayer - 
//			*pObject - 
//-----------------------------------------------------------------------------
void CPlayerPickupController::Init( CBaseHoloHand *pHand, CBaseEntity *pObject )
{
	// If the target is debris, convert it to non-debris
	if ( pObject->GetCollisionGroup() == COLLISION_GROUP_DEBRIS )
	{
		// Interactive debris converts back to debris when it comes to rest
		pObject->SetCollisionGroup( COLLISION_GROUP_INTERACTIVE_DEBRIS );
	}

	// done so I'll go across level transitions with the player
	SetParent( pHand );
	m_grabController.SetIgnorePitch( true );
	m_grabController.SetAngleAlignment( DOT_30DEGREE );
	m_pHand = pHand;
	IPhysicsObject *pPhysics = pObject->VPhysicsGetObject();
	
	Pickup_OnPhysGunPickup( pObject, pHand->GetOwnerPlayer(), PICKED_UP_BY_PLAYER );
	
	m_grabController.AttachEntity( pHand, pObject, pPhysics, false, vec3_origin, false );
	
	pHand->SetUseEntity( this );
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : bool - 
//-----------------------------------------------------------------------------
void CPlayerPickupController::Shutdown( bool bThrown )
{
	CBaseEntity *pObject = m_grabController.GetAttached();

	bool bClearVelocity = false;
	if ( !bThrown && pObject && pObject->VPhysicsGetObject() && pObject->VPhysicsGetObject()->GetContactPoint(NULL,NULL) )
	{
		bClearVelocity = true;
	}

	m_grabController.DetachEntity( bClearVelocity );
	if ( pObject != NULL )
	{
		Pickup_OnPhysGunDrop( pObject, m_pHand->GetOwnerPlayer(), bThrown ? THROWN_BY_PLAYER : DROPPED_BY_PLAYER );
	}

	if ( m_pHand )
	{
		m_pHand->SetUseEntity( NULL );
	}

	Remove();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CPlayerPickupController::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( pActivator == m_pHand )
	{
		CBaseEntity *pAttached = m_grabController.GetAttached();
		if( !pAttached )
		{
			Shutdown();
			return;
		}

		//Adrian: Oops, our object became motion disabled, let go!
		IPhysicsObject *pPhys = pAttached->VPhysicsGetObject();
		if ( pPhys && pPhys->IsMoveable() == false )
		{
			Shutdown();
			return;
		}

		// UNDONE: Use vphysics stress to decide to drop objects
		// UNDONE: Must fix case of forcing objects into the ground you're standing on (causes stress) before that will work
		if ( !pAttached || useType == USE_OFF || m_grabController.ComputeError() > 12 )
		{
			Shutdown( true );
			Vector vecLaunch;
			
			// HOLODECK: Base launch direction is now the (eye->hand) vector.
			CGridPlayer *gridplayer = dynamic_cast<CGridPlayer *>( m_pHand->GetOwnerPlayer() );
			Assert( gridplayer );
			vecLaunch = m_pHand->GetAbsOrigin() - gridplayer->EyePosition();
			vecLaunch.NormalizeInPlace();

			// JAY: Scale this with mass because some small objects really go flying
			float massFactor = clamp( pPhys->GetMass(), 0.5, 15 );
			massFactor = RemapVal( massFactor, 0.5, 15, 0.5, 4 );
			vecLaunch *= player_throwforce.GetFloat() * massFactor;

			// HOLODECK: made the launch velocity affected by the hand velocity.
			Vector handVelocity = m_pHand->GetHoloHand().GetVelocity();
			
			// Ensure the sign of `vecLaunch` is preserved.
			handVelocity.x = fabs( handVelocity.x );
			handVelocity.y = fabs( handVelocity.y );
			handVelocity.z = fabs( handVelocity.z );
			vecLaunch *= handVelocity;

			pPhys->ApplyForceCenter( vecLaunch );
			AngularImpulse aVel = RandomAngularImpulse( -10, 10 ) * massFactor;
			pPhys->ApplyTorqueCenter( aVel );
			return;
		}

#if STRESS_TEST
		vphysics_objectstress_t stress;
		CalculateObjectStress( pPhys, pAttached, &stress );
		if ( stress.exertedStress > 250 )
		{
			Shutdown();
			return;
		}
#endif

		if ( useType == USE_SET )
		{
			// update position
			m_grabController.UpdateObject( m_pHand, 12 );
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CPlayerPickupController::IsHoldingEntity( CBaseEntity *pEnt )
{
	return ( m_grabController.GetAttached() == pEnt );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool PlayerPickupControllerIsHoldingEntity( CBaseEntity *pPickupControllerEntity, CBaseEntity *pHeldEntity )
{
	CPlayerPickupController *pController = dynamic_cast<CPlayerPickupController *>(pPickupControllerEntity);

	return pController ? pController->IsHoldingEntity( pHeldEntity ) : false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CBaseEntity *GetPlayerHeldEntity( CBasePlayer *pPlayer )
{
	CBaseEntity *pObject = NULL;
	CPlayerPickupController *pPlayerPickupController = (CPlayerPickupController *)(pPlayer->GetUseEntity());

	if ( pPlayerPickupController )
	{
		pObject = pPlayerPickupController->GetGrabController().GetAttached();
	}

	return pObject;
}