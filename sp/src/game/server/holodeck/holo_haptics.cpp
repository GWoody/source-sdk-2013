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
	_power = _frequency = 0;
	_enabled = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CHoloHapticEvent::~CHoloHapticEvent()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
BEGIN_SEND_TABLE_NOBASE( CHoloHaptics, DT_HoloHaptics )
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
CHoloHaptics::CHoloHaptics() :
	_events( 0, 0, HapticEventLessFunc )
{
	_power = _frequency = 0;
	_enabled = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHaptics::PushEvent( CHoloHapticEvent *event )
{
	_events.Insert( event );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHaptics::Update()
{
	// Run all events. Clear off those that have finished.
	for( int i = 0; i < _events.Count(); i++ )
	{
		CHoloHapticEvent *event = _events.Element( i );
		if( !event->Update() )
		{
			_events.RemoveAt( i );
			delete event;
		}
	}

	if( !_events.Count() )
	{
		// More more events. Kill the device.
		Disable();
	}
	else
	{
		// Set the highest priority event active.
		CHoloHapticEvent *event = _events.Element( 0 );
		SetPower( event->GetPower() );
		SetFrequency( event->GetFrequency() );
		SetEnabled( event->IsEnabled() );
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