/*
===============================================================================

	grid_haptic_events.cpp
	Implements all haptic event types for Grid.

===============================================================================
*/

#include "cbase.h"
#include "grid_haptic_events.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CWeaponShootHapticEvent::CWeaponShootHapticEvent( unsigned char power, unsigned int freq ) : CHoloHapticEvent( SHOOT )
{
	_weaponPower = power;
	_weaponFreq = freq;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CWeaponShootHapticEvent::Update()
{
	if( !_enabled )
	{
		// This event shouldn't be removed, only nullified.
		Clear();
		return true;
	}

	_power = _weaponPower;
	_frequency = _weaponFreq;
	_enabled = true;

	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CWeaponShootHapticEvent::Enable()
{
	_enabled = true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CWeaponShootHapticEvent::Disable()
{
	_enabled = false;
}