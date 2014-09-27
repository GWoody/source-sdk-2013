/*
===============================================================================

	holo_haptics.cpp
	Implements the interface for the Holodeck haptic class.

===============================================================================
*/

#include "cbase.h"
#include "holo_haptics.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool HapticEventLessFunc( CHoloHapticEvent * const &a, CHoloHapticEvent * const &b )
{
	return a->GetPower() < b->GetPriority();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CHoloHapticEvent::CHoloHapticEvent( EPriority priority ) :
	_priority( priority )
{
	Clear();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CHoloHapticEvent::~CHoloHapticEvent()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticEvent::Clear()
{
	_power = _frequency = 0;
	_enabled = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
BEGIN_SEND_TABLE_NOBASE( CHoloHaptics, DT_HoloHaptics )

	SendPropInt( SENDINFO(_target) ),
	SendPropInt( SENDINFO(_power) ),
	SendPropInt( SENDINFO(_frequency) ),
	SendPropBool( SENDINFO(_enabled) ),

END_SEND_TABLE()

BEGIN_SIMPLE_DATADESC( CHoloHaptics )

	DEFINE_FIELD( _power, FIELD_INTEGER ),
	DEFINE_FIELD( _frequency, FIELD_INTEGER ),
	DEFINE_FIELD( _enabled, FIELD_BOOLEAN ),

END_DATADESC()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CHoloHaptics::CHoloHaptics( int target ) :
	_events( 0, 0, HapticEventLessFunc )
{
	_target = target;
	_power = _frequency = 0;
	_enabled = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHaptics::AddEvent( CHoloHapticEvent *event )
{
	_events.Insert( event );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHaptics::RemoveEvent( CHoloHapticEvent *event )
{
	for( int i = 0; i < _events.Count(); i++ )
	{
		if( _events.Element(i) == event )
		{
			_events.RemoveAt( i );
			break;
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHaptics::Update()
{
	unsigned char maxpower = 0;
	unsigned char maxfreq = 0;
	bool enabled = false;

	// Run all events. Clear off those that have finished.
	for( int i = 0; i < _events.Count(); i++ )
	{
		CHoloHapticEvent *event = _events.Element( i );
		if( !event->Update() )
		{
			_events.RemoveAt( i );
			delete event;
		}
		else if( event->IsEnabled() )
		{
			maxpower = max( maxpower, event->GetPower() );
			maxfreq = max( maxfreq, event->GetFrequency() );
			enabled = true;
		}
	}

	if( !_events.Count() )
	{
		// More more events. Kill the device.
		Disable();
	}
	else
	{
		SetPower( maxpower );
		SetFrequency( maxfreq );
		SetEnabled( enabled );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHaptics::Disable()
{
	SetPower( 0 );
	SetFrequency( 0 );
	SetEnabled( false );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHaptics::ClearAllEvents()
{
	_events.RemoveAll();
	Disable();
}