/*
==============================================================================

	out_etactor.cpp
	Implements the haptic feedback interface. 

==============================================================================
*/

#include "cbase.h"
#include "out_etactor.h"

#pragma warning( push )
#pragma warning( disable : 4005 )
	#define _WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#pragma warning( pop )

#include "tenslib.h"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static const int TENS_TIMEOUT_MS = 20;
static ConVar holo_target_etactor( "holo_target_etactor", "1", FCVAR_HIDDEN | FCVAR_ARCHIVE );

//----------------------------------------------------------------------------
// Thread that actually updates the device.
//----------------------------------------------------------------------------
class CETactorThread : public CThread
{
public:
	virtual int Run()
	{
		_quit = false;

		while( !_quit )
		{
			// We set target to 0 when we commit a new target.
			// Therefore, if another thread is modifying the target it will be non zero.
			if( _target && _targetMutex.TryLock() )
			{
				tens_settarget( _target );
				_target = 0;
				_targetMutex.Lock();
				Sleep( TENS_TIMEOUT_MS );
			}

			if( _enabledMutex.TryLock() )
			{
				tens_enable( TRUE, TRUE );
				_enabledMutex.Unlock();
				Sleep( TENS_TIMEOUT_MS );
			}

			if( _powerMutex.TryLock() )
			{
				tens_power( _power );
				_powerMutex.Unlock();
				Sleep( TENS_TIMEOUT_MS );
			}

			if( _freqMutex.TryLock() )
			{
				tens_freq( _frequency );
				_freqMutex.Unlock();
				Sleep( TENS_TIMEOUT_MS );
			}
		}

		return 0;
	}

	void			SetPower( unsigned char power )		{ _powerMutex.Lock(); _power = power; _powerMutex.Unlock(); }
	void			SetFrequency( unsigned char freq )	{ _freqMutex.Lock(); _frequency = freq; _freqMutex.Unlock(); }
	void			SetEnabled( bool enabled )			{ _enabledMutex.Lock(); _enabled = enabled; _enabledMutex.Unlock(); }
	void			SetTarget( unsigned char target )	{ _targetMutex.Lock(); _target = target; _targetMutex.Unlock(); }
	void			Quit()		{ _quit = true; }

private:
	// ETactor state.
	volatile unsigned char	_power;
	volatile unsigned char	_frequency;
	volatile bool	_enabled;
	volatile unsigned char	_target;

	volatile bool	_quit;

	CThreadMutex	_powerMutex;
	CThreadMutex	_freqMutex;
	CThreadMutex	_enabledMutex;
	CThreadMutex	_targetMutex;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class CETactor : public IETactor
{
public:
	CETactor();
	~CETactor();

	virtual void	SetPower( unsigned char power );
	virtual void	SetFrequency( unsigned char freq );
	virtual void	SetEnabled( bool enabled );
	virtual void	SetTarget( unsigned char target );

	virtual void	Connect();

private:
	CETactorThread	_thread;
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
	Connect();
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
void CETactor::SetPower( unsigned char power )
{
	_thread.SetPower( power );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CETactor::SetFrequency( unsigned char freq )
{
	_thread.SetFrequency( freq );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CETactor::SetEnabled( bool enabled )
{
	_thread.SetEnabled( enabled );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CETactor::SetTarget( unsigned char target )
{
	_thread.SetTarget( target );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CETactor::Connect()
{
	_thread.Quit();
	_thread.Join();

	ConColorMsg( COLOR_GREEN, "Connecting to ETactor ID %d\n", holo_target_etactor.GetInt() );

	// Ready the device connection.
	if( !tens_init( "com3" ) )
	{
		ConColorMsg( COLOR_YELLOW, "Failed to open connection to ETactor!\n" );
		return;
	}
	Sleep( TENS_TIMEOUT_MS );

	// Connect to the device.
	int target = holo_target_etactor.GetInt() >= 1 ? holo_target_etactor.GetInt() : 1;
	tens_settarget( target );
	Sleep( TENS_TIMEOUT_MS );

	// Ready power settings.
	tens_power( 0 );
	Sleep( TENS_TIMEOUT_MS );
	tens_freq( 0 );
	Sleep( TENS_TIMEOUT_MS );

	_thread.Start();
	ConColorMsg( COLOR_GREEN, "ETactor initialized.\n" );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CON_COMMAND( holo_etactor_set_target, "Sets the ID of the target ETactor device" )
{
	if( args.ArgC() < 2 )
	{
		Warning( "holo_set_target_etactor [etactor_id]\n" );
		return;
	}

	unsigned char target = atoi( args[1] ) & 0xFF;
	IETactor::Get().SetTarget( target );
	ConColorMsg( COLOR_GREEN, "Now targeting ETactor ID %d\n", target );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CON_COMMAND( holo_etactor_connect, "Opens a connection to the current ETactor ID, or the specified ID.\n" )
{
	if( args.ArgC() > 1 )
	{
		// Connect to the specified ID.
		unsigned char target = atoi( args[1] ) & 0xFF;
		holo_target_etactor.SetValue( target );
	}

	IETactor::Get().Connect();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CON_COMMAND( holo_etactor_setpower, "Sets the power level for the current ETactor.\n" )
{
	if( args.ArgC() < 2 )
	{
		Warning( "holo_etactor_setpower [0, 255]\n" );
		return;
	}

	unsigned char parm = atoi( args[1] ) & 0xFF;
	IETactor::Get().SetPower( parm );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CON_COMMAND( holo_etactor_setfreq, "Sets the discharge frequency for the current ETactor.\n" )
{
	if( args.ArgC() < 2 )
	{
		Warning( "holo_etactor_setfreq [0, 255]\n" );
		return;
	}

	unsigned char parm = atoi( args[1] ) & 0xFF;
	IETactor::Get().SetFrequency( parm );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CON_COMMAND( holo_etactor_setenabled, "Turns the current ETactor on or off.\n" )
{
	if( args.ArgC() < 2 )
	{
		Warning( "holo_etactor_setfreq [0, 255]\n" );
		return;
	}

	bool parm = atoi( args[1] ) ? true : false;
	IETactor::Get().SetEnabled( parm );
}