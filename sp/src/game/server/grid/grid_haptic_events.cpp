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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CShockDmgHapticEvent::CShockDmgHapticEvent() : CHoloHapticEvent( TAKE_DAMAGE )
{
	_startTime = gpGlobals->curtime;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CShockDmgHapticEvent::Update()
{
	if( _startTime + 0.5f < gpGlobals->curtime )
	{
		Clear();
		return false;
	}

	_power = 127;
	_frequency = 255;
	_enabled = true;

	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CBulletDmgHapticEvent::CBulletDmgHapticEvent() : CHoloHapticEvent( TAKE_DAMAGE )
{
	_startTime = gpGlobals->curtime;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CBulletDmgHapticEvent::Update()
{
	if( _startTime + 0.3f < gpGlobals->curtime )
	{
		Clear();
		return false;
	}

	_power = 191;
	_frequency = 127;
	_enabled = true;

	return true;
}