/*
===============================================================================

	holo_haptics.h
	Defines the interface for the Holodeck haptic class.

===============================================================================
*/

#ifndef __HOLO_HAPTICS_H__
#define __HOLO_HAPTICS_H__

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CHoloHapticEvent
{
public:
	enum EPriority
	{
		USE_PANEL,
		PICKUP_OBJECT,
		SHOOT,
		TAKE_DAMAGE,

		PRIORITY_COUNT
	};

	CHoloHapticEvent( EPriority priority );
	virtual ~CHoloHapticEvent();

	// Accessors.
	EPriority		GetPriority() const		{ return _priority; }
	unsigned char	GetPower() const		{ return _power; }
	unsigned char	GetFrequency() const	{ return _frequency; }
	bool			IsEnabled() const		{ return _enabled; }

	virtual bool	Update() = 0;

protected:
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

	void			PushEvent( CHoloHapticEvent *event );
	void			Update();
	void			Disable();

	void			SetPower( unsigned char power )			{ _power = power; }
	void			SetFrequency( unsigned char freq )		{ _frequency = freq; }
	void			SetEnabled( bool enabled )				{ _enabled = enabled; }

private:
	CUtlPriorityQueue<CHoloHapticEvent *>	_events;

	CNetworkVar( int, _power );
	CNetworkVar( int, _frequency );
	CNetworkVar( bool, _enabled );
};

EXTERN_SEND_TABLE( DT_HoloHaptics );

#endif // __HOLO_HAPTICS_H__