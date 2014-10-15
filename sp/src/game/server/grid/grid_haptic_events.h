/*
===============================================================================

	grid_haptic_events.h
	Defines all haptic event types for Grid.

===============================================================================
*/

#ifndef __GRID_HAPTIC_EVENTS_H__
#define __GRID_HAPTIC_EVENTS_H__

#include "holodeck/holo_haptics.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CWeaponShootHapticEvent : public CHoloHapticEvent
{
public:
	CWeaponShootHapticEvent( unsigned char power, unsigned int freq );

	void			Enable();
	void			Disable();

	virtual bool	Update();

private:
	unsigned char	_weaponPower;
	unsigned char	_weaponFreq;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CShockDmgHapticEvent : public CHoloHapticEvent
{
public:
	CShockDmgHapticEvent();

	virtual bool	Update();

private:
	float			_startTime;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CBulletDmgHapticEvent : public CHoloHapticEvent
{
public:
	CBulletDmgHapticEvent();

	virtual bool	Update();

private:
	float			_startTime;
};

#endif // __GRID_HAPTIC_EVENTS_H__