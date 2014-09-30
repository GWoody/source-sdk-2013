/*
===============================================================================

	grid_environment_light.cpp
	Implements the sun\moon lighting system for Grid.

===============================================================================
*/

#include "cbase.h"
#include "sunlightshadowcontrol.h"
#include "grid_sun.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridEnvironmentLight : public CSunlightShadowControl
{
public:
	DECLARE_CLASS( CGridEnvironmentLight, CSunlightShadowControl );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	virtual void	Spawn();
	virtual void	Think();

	void			InputSetMaxPitch( inputdata_t &data );

private:
	float			_maxPitch;

	CHandle<CGridSun>	_sunSprite;

	float			_hour;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( grid_environment_light, CGridEnvironmentLight );

IMPLEMENT_SERVERCLASS_ST( CGridEnvironmentLight, DT_GridEnvironmentLight )
END_SEND_TABLE()

BEGIN_DATADESC( CGridEnvironmentLight )

	DEFINE_INPUTFUNC( FIELD_FLOAT, "SetMaxPitch", InputSetMaxPitch ),

END_DATADESC();


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridEnvironmentLight::Spawn( void )
{
	BaseClass::Spawn();

	//
	// Selt projected texture variables.
	//
	m_flSunDistance = 29000.0f;
	m_flFOV = 10.0f;
	m_flNearZ = 2048.0f;
	m_flNorthOffset = 512.0f;
	m_bEnableShadows = true;

	//
	// Configure sun.
	//
	_maxPitch = 45;
	_hour = 0.0f;
	_sunSprite = (CGridSun *)CreateEntityByName( "grid_env_sun" );

	SetNextThink( gpGlobals->curtime + 0.1f );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridEnvironmentLight::Think( void )
{	
	float yaw = _hour / 12.0f * 180.0f;
	float pitch = 0;

	_hour += gpGlobals->frametime;

	if( _hour >= 12.0f )
	{
		_hour = 0.0f;
	}
	
	const float MAX_PITCH_SQR = _maxPitch * _maxPitch;
	const float TIME_SQR = (_hour - 6) * (_hour - 6);
	const float TOTAL_TIME_SQR = 6.0f * 6.0f;
	pitch = sqrt( MAX_PITCH_SQR * ( 1 - ( TIME_SQR / TOTAL_TIME_SQR ) ) );

	_sunSprite->SetPitchYaw( pitch, yaw );
	m_shadowDirection = _sunSprite->GetLightDirection();

	BaseClass::Think();
	SetNextThink( gpGlobals->curtime + 0.1f );	
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridEnvironmentLight::InputSetMaxPitch( inputdata_t &data )
{
	_maxPitch = data.value.Float();
}