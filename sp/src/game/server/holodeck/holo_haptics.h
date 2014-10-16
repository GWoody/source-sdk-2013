/*
===============================================================================

	holo_haptics.h
	Defines the interface for the Holodeck haptic class.

===============================================================================
*/

#ifndef __HOLO_HAPTICS_H__
#define __HOLO_HAPTICS_H__

#include "holodeck/holo_shared.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CHoloHapticEvent
{
public:
	enum EPriority
	{
		ENVIRONMENT,
		USE_PANEL,
		PICKUP_OBJECT,
		SHOOT,
		TAKE_DAMAGE,

		PRIORITY_COUNT
	};

	CHoloHapticEvent( EPriority priority );
	virtual ~CHoloHapticEvent();

	void			Clear();

	void			SetId( unsigned int id )	{ _id = id; }

	// Accessors.
	EPriority		GetPriority() const		{ return _priority; }
	unsigned char	GetPower() const		{ return _power; }
	unsigned char	GetFrequency() const	{ return _frequency; }
	bool			IsEnabled() const		{ return _enabled; }
	unsigned int	GetId() const			{ return _id; }

	virtual bool	Update() = 0;

protected:
	unsigned int	_id;

	unsigned char	_power;
	unsigned char	_frequency;
	bool			_enabled;

	EPriority		_priority;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CHoloHaptics
{
public:
	DECLARE_SIMPLE_DATADESC();
	DECLARE_CLASS_NOBASE( CHoloHaptics );
	DECLARE_EMBEDDED_NETWORKVAR();

	CHoloHaptics();

	void			AddEvent( CHoloHapticEvent *event );
	void			RemoveEvent( CHoloHapticEvent *event );
	void			ClearAllEvents();

	void			SetActive( bool active )				{ _active = active; }
	void			SetTargetHand( EHand hand )				{ _targetHand = hand; }

	void			Update();

protected:
	void			Disable();

	void			SetPower( unsigned char power )			{ _power = power; }
	void			SetFrequency( unsigned char freq )		{ _frequency = freq; }
	void			SetEnabled( bool enabled )				{ _enabled = enabled; }

private:
	CUtlPriorityQueue<CHoloHapticEvent *>	_events;

	CNetworkVar( int, _targetHand );
	CNetworkVar( int, _power );
	CNetworkVar( int, _frequency );
	CNetworkVar( bool, _enabled );
	bool			_active;
};

EXTERN_SEND_TABLE( DT_HoloHaptics );

#endif // __HOLO_HAPTICS_H__