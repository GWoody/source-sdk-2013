/*
===============================================================================

	base_grid_player.cpp
    Implements player functionality borrowed from HL2.

===============================================================================
*/

#include "cbase.h"
#include "base_grid_player.h"
#include "ai_basenpc.h"
#include "in_buttons.h"

//-----------------------------------------------------------------------------
// External functions.
//-----------------------------------------------------------------------------
extern bool PlayerPickupControllerIsHoldingEntity( CBaseEntity *pPickupController, CBaseEntity *pHeldEntity );
extern float PlayerPickupGetHeldObjectMass( CBaseEntity *pPickupControllerEntity, IPhysicsObject *pHeldObject );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CBaseGridPlayer::CBaseGridPlayer()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CBaseGridPlayer::IsHoldingEntity( CBaseEntity *pEnt )
{
	return PlayerPickupControllerIsHoldingEntity( m_hUseEntity, pEnt );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float CBaseGridPlayer::GetHeldObjectMass( IPhysicsObject *pHeldObject )
{
	return PlayerPickupGetHeldObjectMass( m_hUseEntity, pHeldObject );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CBaseGridPlayer::PlayerUse ( void )
{
	// Nothing here.
	// Don't chain to base.
}