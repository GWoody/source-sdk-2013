/*
===============================================================================

	base_pickupmanager.cpp
		Mostly borrowed from `hl2/weapon_physcannon.cpp`

===============================================================================
*/

#include "cbase.h"
#include "base_pickupcontroller.h"
#include "base_grabcontroller.h"

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pEnt - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
void PlayerPickupObject( CBasePlayer *pPlayer, CBaseEntity *pObject )
{
	//Don't pick up if we don't have a phys object.
	if ( pObject->VPhysicsGetObject() == NULL )
		 return;

	CPlayerPickupController *pController = (CPlayerPickupController *)CBaseEntity::Create( "player_pickup", pObject->GetAbsOrigin(), vec3_angle, pPlayer );
	
	if ( !pController )
		return;

	pController->Init( pPlayer, pObject );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float PlayerPickupGetHeldObjectMass( CBaseEntity *pPickupControllerEntity, IPhysicsObject *pHeldObject )
{
	float mass = 0.0f;
	CPlayerPickupController *pController = dynamic_cast<CPlayerPickupController *>(pPickupControllerEntity);
	if ( pController )
	{
		CGrabController &grab = pController->GetGrabController();
		mass = grab.GetSavedMass( pHeldObject );
	}
	return mass;
}