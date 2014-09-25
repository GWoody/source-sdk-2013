/*
==============================================================================

	out_etactor.cpp
	Implements the haptic feedback interface. 

==============================================================================
*/

#include "cbase.h"
#include "out_etactor.h"

#include "tenslib.h"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
static ConVar holo_target_etactor( "holo_target_etactor", "1", FCVAR_HIDDEN | FCVAR_ARCHIVE );
CON_COMMAND( holo_set_target_etactor, "Sets the ID of the target ETactor device" )
{
	if( args.ArgC() < 2 )
	{
		Warning( "holo_set_target_etactor [etactor_id]\n" );
		return;
	}

	IETactor::Get().SetTarget( atoi(args[1]) & 0xFF );
	ConColorMsg( COLOR_GREEN, "Now targetting ETactor ID %d\n", args[1] );
}

//----------------------------------------------------------------------------
// Thread that actually updates the device.
//----------------------------------------------------------------------------
class CETactorThread : public CThread
{
public:
	const int TENS_TIMEOUT_MS = 20;

	virtual int Run()
	{
		_quit = false;

		// Ready the device connection.
		tens_init( "com3" );
		Sleep( TENS_TIMEOUT_MS );

		// Connect to the device.
		int target = holo_target_etactor.GetInt() >= 1 ? holo_target_etactor.GetInt() : 1;
		tens_settarget( target );

		// Ready power settings.
		tens_power( 0 );
		Sleep( TENS_TIMEOUT_MS );
		tens_freq( 0 );
		Sleep( TENS_TIMEOUT_MS );

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
	_thread.Start();
	ConColorMsg(COLOR_GREEN, "ETactor initialized.\n" );
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