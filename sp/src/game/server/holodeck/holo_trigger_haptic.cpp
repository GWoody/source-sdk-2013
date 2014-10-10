/*
===============================================================================

	holo_trigger_haptic.cpp
	Implements a trigger volume that fires up the users etactors.

===============================================================================
*/

#include "cbase.h"
#include "holo_trigger_haptic.h"
#include "holo_player.h"
#include "holo_haptic_events.h"

//-----------------------------------------------------------------------------
// Source entity configuration.
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( holo_trigger_haptic, CHoloTriggerHaptic );

BEGIN_DATADESC( CHoloTriggerHaptic )

	DEFINE_KEYFIELD( _power, FIELD_INTEGER, "power" ),
	DEFINE_KEYFIELD( _frequency, FIELD_INTEGER, "frequency" ),

END_DATADESC()

bool event_less_func( const int &a, const int &b )
{
	return a < b;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CHoloTriggerHaptic::CHoloTriggerHaptic() :
	_events( event_less_func )
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CHoloTriggerHaptic::PassesTriggerFilters( CBaseEntity *pOther )
{
	if( !pOther )
	{
		return false;
	}

	CHoloPlayer *pPlayer = dynamic_cast<CHoloPlayer *>( UTIL_GetLocalPlayer() );
	Assert( pPlayer );

	for( int i = 0; i < HAND_COUNT; i++ )
	{
		CBaseEntity *pHand = pPlayer->GetHandEntity( (EHand)i );
		if( !pHand )
		{
			return false;
		}

		// Only allow the hand to interact with Holodeck triggers.
		if( pHand->entindex() == pOther->entindex() )
		{
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloTriggerHaptic::Touch( CBaseEntity *pOther )
{
	if( !PassesTriggerFilters( pOther ) )
	{
		return;
	}

	CHoloHand *hand = (CHoloHand *)pOther;
	int eventIdx = _events.Find( hand->entindex() );
	if( eventIdx == _events.InvalidIndex() )
	{
		CTriggerHapticEvent *haptic = new CTriggerHapticEvent( _power, _frequency );
		_events.Insert( hand->entindex(), haptic );

		hand->GetHaptics().AddEvent( haptic );
	}

	BaseClass::Touch( pOther );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloTriggerHaptic::EndTouch( CBaseEntity *pOther )
{
	CHoloHand *hand = dynamic_cast<CHoloHand *>( pOther );
	if( !hand )
	{
		return;
	}

	int eventIdx = _events.Find( hand->entindex() );
	if( eventIdx != _events.InvalidIndex() )
	{
		CTriggerHapticEvent *haptic = _events.Element( eventIdx );
		hand->GetHaptics().RemoveEvent( haptic );

		_events.RemoveAt( eventIdx );
		delete haptic;
	}

	BaseClass::EndTouch( pOther );
}