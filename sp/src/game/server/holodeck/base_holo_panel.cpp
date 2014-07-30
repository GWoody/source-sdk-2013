/*
===============================================================================

	base_holo_panel.h
		Implements common Holodeck panel behaviour.

===============================================================================
*/

#include "cbase.h"
#include "base_holo_panel.h"

BEGIN_DATADESC( CBaseHoloPanel )

	// Save state.
	DEFINE_FIELD( _glowTarget, FIELD_EHANDLE ),

	// Hammer attributes.
	DEFINE_KEYFIELD( _glowTargetName, FIELD_STRING, "glowtarget" ),

END_DATADESC()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CBaseHoloPanel::Spawn()
{
	BaseClass::Spawn();

	// This entity can only be triggered once every second.
	m_flWait = 1.0f;

	InitEntityGlow();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CBaseHoloPanel::UpdateOnRemove()
{
	RemoveEntityGlow();
	BaseClass::UpdateOnRemove();
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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CBaseHoloPanel::InitEntityGlow()
{
	_glowTarget.Set( NULL );
	if( _glowTargetName != NULL_STRING )
	{
		// Find the entity that we must make glow.
		_glowTarget = gEntList.FindEntityByName( NULL, STRING( _glowTargetName ) );

		if( !_glowTarget )
		{
			// The entity name doesn't exist.
			Warning( "HOLODECK: Gesture panel references invalid glow entity \"%s\"\n", STRING( _glowTargetName ) );
		}
		else
		{
			// Ensure the target entity is a model based entity.
			CBaseAnimating *target = dynamic_cast<CBaseAnimating *>( _glowTarget.Get() );
			if( !target )
			{
				Warning( "HOLODECK: Gesture panel references non studio model glow entity \"%s\"\n", STRING( _glowTargetName ) );
			}
			else
			{
				target->AddGlowEffect();
			}
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CBaseHoloPanel::RemoveEntityGlow()
{
	CBaseAnimating *target = dynamic_cast<CBaseAnimating *>( _glowTarget.Get() );
	if( target )
	{
		target->RemoveGlowEffect();
	}
}