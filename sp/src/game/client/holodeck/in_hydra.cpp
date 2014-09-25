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

	// Calibration methods.
	void			MarkHome( const sixenseControllerData &data );

	// Source methods.
	virtual void	CreateMove( CUserCmd *cmd );

protected:
	Vector			HydraToSourceVector( const float *v );
	float			HydraToSourceDistance( float distance );

private:
	bool			_initialized;

	Vector			_home;
};

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
		ConColorMsg( COLOR_YELLOW, "Failed to initialize SixenseSDK!\n" );
		_initialized = false;
		return;
	}

	sixenseSetFilterEnabled( 1 );

	ConColorMsg( COLOR_GREEN, "Initialized SixenseSDK.\n" );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CRazerHydra::~CRazerHydra()
{
	sixenseExit();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CRazerHydra::MarkHome( const sixenseControllerData &data )
{
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

	if( data.buttons )
	{
		MarkHome( data );
	}

	Vector curPos = HydraToSourceVector( data.pos );

	cmd->viewoffset = curPos - _home;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
Vector CRazerHydra::HydraToSourceVector( const float *v )
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