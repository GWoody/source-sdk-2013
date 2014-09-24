/*
==============================================================================

	in_hydra.cpp
	Implements the Hydra head tracking interface. 

==============================================================================
*/

#include "cbase.h"
#include "in_hydra.h"

#include "sixense.h"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class CRazerHydra : public IRazerHydra
{
public:
	CRazerHydra();
	~CRazerHydra();

	// Singleton methods.
	static CRazerHydra &	Get()			{ return *(CRazerHydra *)&IRazerHydra::Get(); }

	// Calibration methods.
	void			MarkHome();

	// Source methods.
	virtual void	CreateMove( CUserCmd *cmd );

protected:
	Vector			HydraToSourceVector( float *v );
	float			HydraToSourceDistance( float distance );

private:
	bool			_initialized;

	Vector			_home;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CON_COMMAND( holo_set_hydra_home, "Marks the current Hydra controller position as the home position." )
{
	CRazerHydra::Get().MarkHome();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
IRazerHydra *IRazerHydra::_instance = NULL;
void IRazerHydra::Create()
{
	_instance = new CRazerHydra;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CRazerHydra::CRazerHydra()
{
	_initialized = true;
	_home.Init( 0, 0, 0 );

	if( sixenseInit() == SIXENSE_FAILURE )
	{
		ConColorMsg( COLOR_YELLOW, "Failed to initialize Razer Hydra!\n" );
		_initialized = false;
		return;
	}

	sixenseSetFilterEnabled( 1 );

	ConColorMsg( COLOR_GREEN, "Initialized Razer Hydra." );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CRazerHydra::~CRazerHydra()
{
	sixenseExit();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CRazerHydra::MarkHome()
{
	if( !_initialized || !sixenseIsBaseConnected( 0 ) )
	{
		ConColorMsg( COLOR_YELLOW, "Hydra module isn't initialized!\n" );
		return;
	}

	sixenseControllerData data;
	if( sixenseGetNewestData( 0, &data ) == SIXENSE_FAILURE )
	{
		ConColorMsg( COLOR_YELLOW, "Failed to retrive data from Hydra controller 0!\n" );
		return;
	}

	if( !data.enabled )
	{
		ConColorMsg( COLOR_YELLOW, "Hydra controller 0 is not enabled!\n" );
		return;
	}

	_home = HydraToSourceVector( data.pos );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CRazerHydra::CreateMove( CUserCmd *cmd )
{
	if( !_initialized || !sixenseIsBaseConnected( 0 ) )
	{
		return;
	}

	sixenseControllerData data;
	if( sixenseGetNewestData( 0, &data ) == SIXENSE_FAILURE )
	{
		return;
	}

	if( !data.enabled || data.is_docked )
	{
		return;
	}

	Vector curPos = HydraToSourceVector( data.pos );

	cmd->viewoffset = curPos - _home;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
Vector CRazerHydra::HydraToSourceVector( float *v )
{
	// Source uses	{ forward, left, up }.
	// Hydra uses	{ right, up, back }.
	return Vector( -HydraToSourceDistance( v[2] ), -HydraToSourceDistance( v[0] ), HydraToSourceDistance( v[1] ) );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float CRazerHydra::HydraToSourceDistance( float distance )
{
	// Hydra uses millimeters, Source uses inches.
	// The player is 72 units tall, which is estimated to be [5ft 10in]\[1.778m].
	// This gives us a meters to Source unit factor of:
	static const float scaleFactor = 1.778f / 72.0f;	// meters / unit
	return distance * scaleFactor;
}