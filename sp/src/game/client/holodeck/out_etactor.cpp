/*
==============================================================================

	out_etactor.cpp
	Implements the haptic feedback interface. 

==============================================================================
*/

#include "cbase.h"
#include "out_etactor.h"
#include "out_etactor_device.h"

#pragma warning( push )
#pragma warning( disable : 4005 )
	#define _WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#pragma warning( pop )

#include "tenslib.h"

//----------------------------------------------------------------------------
// Thread that actually updates the device.
//----------------------------------------------------------------------------
class CETactorThread : public CThread
{
public:
	virtual int Run()
	{
		_insertLock = false;
		_quit = false;

		while( !_quit )
		{
			while( _insertLock );
			for( int i = 0; i != _devices.Count(); i++ )
			{
				CETactorDevice &device = _devices[i];
				device.Commit();

				while( _insertLock );
			}
		}

		return 0;
	}

	void Insert( etactorId_t target )
	{
		CETactorDevice device( target, this );

		_insertLock = true;
			_devices.AddToTail( device );
		_insertLock = false;
	}

	void SetState( unsigned char target, bool enabled, unsigned char power, unsigned char freq )
	{
		for( int i = 0; i != _devices.Count(); i++ )
		{
			CETactorDevice &device = _devices[i];
			if( device.GetId() == target )
			{
				device.Update( enabled, power, freq );
			}
		}
	}

	void Quit()								
	{
		_quit = true;
	}

private:
	// ETactor state.
	CUtlVector<CETactorDevice> _devices;
	volatile bool	_insertLock;

	volatile bool	_quit;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class CETactor : public IETactor
{
public:
	CETactor();
	~CETactor();

	virtual void	SetState( unsigned char target, bool enabled, unsigned char power, unsigned char freq );

private:
	CETactorThread	_thread;
	CUtlVector<etactorId_t>	_connections;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
IETactor *IETactor::_instance = NULL;
void IETactor::Create()
{
	_instance = new CETactor;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CETactor::CETactor()
{
	// Ready the device connection.
	if( !tens_init( "com3" ) )
	{
		ConColorMsg( COLOR_YELLOW, "Failed to open connection to ETactor!\n" );
		return;
	}

	ConColorMsg( COLOR_GREEN, "Initialized TENSLib\n" );
	Sleep( TENS_TIMEOUT_MS );

	_thread.Start();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CETactor::~CETactor()
{
	_thread.Quit();
	_thread.Join();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CETactor::SetState( unsigned char target, bool enabled, unsigned char power, unsigned char freq )
{
	if( _connections.Find( target ) == _connections.InvalidIndex() )
	{
		ConColorMsg( COLOR_GREEN, "Adding ETactor (id %d) to recipient list\n", target );
		_thread.Insert( target );
		_connections.AddToHead( target );
	}

	_thread.SetState( target, enabled, power, freq );
}