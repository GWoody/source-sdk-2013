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
	template<typename T>
	struct etactorVar
	{
		etactorVar()
		{
			val = 0;
			dirty = true;
		}

		T			val;
		bool		dirty;
	};

public:
	CETactorThread()
	{
		_target.val = holo_target_etactor.GetInt();
	}

	virtual int Run()
	{
		_quit = false;

		while( !_quit )
		{
			if( _target.dirty )
			{
#				pragma omp atomic
				unsigned char target = _target.val;

				tens_settarget( target );

#				pragma omp atomic
				_target.dirty = false;

				Sleep( TENS_TIMEOUT_MS );
			}

			if( _enabled.dirty )
			{
#				pragma omp atomic
				bool enabled = _enabled.val;

				tens_enable( enabled, enabled );

#				pragma omp atomic
				_enabled.dirty = false;

				Sleep( TENS_TIMEOUT_MS );
			}

			if( _powfreq.dirty )
			{
#				pragma omp atomic
				unsigned short powfreq = _powfreq.val;

				unsigned char power = ( powfreq >> 8 ) & 0xFF;
				unsigned char freq = powfreq & 0xFF;
				tens_control( power, freq );

#				pragma omp atomic
				_powfreq.dirty = false;

				Sleep( TENS_TIMEOUT_MS );
			}
		}

		return 0;
	}

	void SetPowerFrequency( unsigned char power, unsigned char freq )
	{ 
		short powfreq = ( power << 8 ) | freq;
		InternalSet( _powfreq, powfreq ); 
	}

	void SetPower( unsigned char power )
	{
		short powfreq = ( power << 8 ) | ( _powfreq.val & 0xFF );
		InternalSet( _powfreq, powfreq ); 
	}

	void SetFrequency( unsigned char freq )
	{
		short powfreq = ( ( _powfreq.val >> 8 ) & 0xFF ) | ( freq );
		InternalSet( _powfreq, powfreq ); 
	}

	void SetTarget( unsigned char target )	{ InternalSet( _target, target ); }
	void SetEnabled(bool enabled)			{ InternalSet( _enabled, enabled ); }
	void Quit()								{ _quit = true; }

private:
	template<typename T> void InternalSet( etactorVar<T> &var, T &val )
	{
#		pragma omp atomic
		T localVal = var.val;

		if( localVal == val )
		{
			return;
		}
		
#		pragma omp atomic
		var.val = val;

#		pragma omp atomic
		var.dirty = true;
	}

	// ETactor state.
	etactorVar<short>	_powfreq;
	etactorVar<bool>	_enabled;
	etactorVar<unsigned char>	_target;

	volatile bool	_quit;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class CETactor : public IETactor
{
public:
	CETactor();
	~CETactor();

	virtual void	SetPowerFrequency( unsigned char power, unsigned char freq );
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
void CETactor::SetPowerFrequency( unsigned char power, unsigned char freq )
{
	_thread.SetPowerFrequency( power, freq );
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

	tens_chargerate( 7 );
	Sleep( TENS_TIMEOUT_MS );

	// Ready power settings.
	tens_power( 0 );
	Sleep( TENS_TIMEOUT_MS );
	tens_freq( 0 );
	Sleep( TENS_TIMEOUT_MS );

	_thread.Start();
	ConColorMsg( COLOR_GREEN, "Connected to ETactor ID %d\n", holo_target_etactor.GetInt() );
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