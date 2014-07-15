/*
===============================================================================

	base_holo_panel.h
		Implements common Holodeck panel behaviour.

===============================================================================
*/

#include "cbase.h"
#include "base_holo_panel.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CBaseHoloPanel::Spawn()
{
	BaseClass::Spawn();

	// This entity can only be triggered once every second.
	m_flWait = 1.0f;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CBaseHoloPanel::PassesTriggerFilters( CBaseEntity *pOther )
{
	if( !pOther )
	{
		return false;
	}

	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
	Assert( pPlayer );

	CBaseEntity *pHand = pPlayer->GetHandEntity();
	if( !pHand )
	{
		return false;
	}

	// Only allow the hand to interact with Holodeck triggers.
	return pHand->entindex() == pOther->entindex();
}