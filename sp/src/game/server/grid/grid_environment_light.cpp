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
	CGridEnvironmentLight();

	virtual void	Spawn();
	virtual void	Think();

	void			InputSetMaxPitch( inputdata_t &data );

private:
	float			CalculatePitch( float localTime );
	void			SetBrightness( float localTime );
	void			SetZoneBrightness( float localTime, const Vector4D &riseBrightess, const Vector4D &horizonBrightess, const Vector4D &brightness );

	CHandle<CGridSun>	_sunSprite;

	float			_maxPitch;
	float			_hour;

	Vector4D		_sunriseBrightness, _sunHorizonBrightness, _sunBrightness;
	Vector4D		_moonriseBrightess, _moonHorizonBrightness, _moonBrightness;
	Vector4D		_blankBrightness;
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
CGridEnvironmentLight::CGridEnvironmentLight()
{
	_sunriseBrightness.Init( 127.0f, 0.0f, 0.0f, 1.0f );
	_sunHorizonBrightness.Init( 237.0f, 200.0f, 127.0f, 1.0f );
	_sunBrightness.Init( 237.0f, 218.0f, 143.0f, 3.0f );

	_moonriseBrightess.Init( 16.0f, 16.0f, 16.0f, 0.1f );
	_moonHorizonBrightness.Init( 32.0f, 32.0f, 32.0f, 0.1f );
	_moonBrightness.Init( 64.0f, 64.0f, 64.0f, 0.15f );

	_blankBrightness.Init( 16.0f, 16.0f, 16.0f, 0.1f );
}

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
	m_flColorTransitionTime = 9999.0f;

	//
	// Configure sun.
	//
	_maxPitch = 45.0f;
	_hour = 0.0f;
	_sunSprite = (CGridSun *)CreateEntityByName( "grid_env_sun" );

	SetNextThink( gpGlobals->curtime + 0.1f );
}

//-----------------------------------------------------------------------------
// Assume a day starts when the sun rises (_hour = 00.00f).
// Assume the night starts at 12.00f hours.
//-----------------------------------------------------------------------------
void CGridEnvironmentLight::Think( void )
{
	_hour += gpGlobals->frametime;

	if( _hour >= 24.0f )
	{
		// Reset the day.
		_hour = _hour - 24.0f;
	}

	// 24 hours has 2 zones: daytime (12 hours) and nighttime (12 hours).
	// Determine the time relative to the current zone.
	float localHour = ( _hour < 12.0f ) ? _hour : _hour - 12.0f;

	//
	// Calculate pitch and yaw.
	//
	{
		float yaw = localHour / 12.0f * 180.0f;
		float pitch = CalculatePitch( localHour );

		_sunSprite->SetPitchYaw( pitch, yaw );
		m_shadowDirection = _sunSprite->GetLightDirection();
	}

	SetBrightness( localHour );

	BaseClass::Think();
	SetNextThink( gpGlobals->curtime + 0.1f );	
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridEnvironmentLight::InputSetMaxPitch( inputdata_t &data )
{
	_maxPitch = data.value.Float();
}

//-----------------------------------------------------------------------------
// Uses the formula for an ellipse, solved for `y`.
//-----------------------------------------------------------------------------
float CGridEnvironmentLight::CalculatePitch( float localTime )
{
	const float MAX_PITCH_SQR = _maxPitch * _maxPitch;
	const float TIME_SQR = (localTime - 6) * (localTime - 6);
	const float TOTAL_TIME_SQR = 6.0f * 6.0f;
	return sqrt( MAX_PITCH_SQR * ( 1 - ( TIME_SQR / TOTAL_TIME_SQR ) ) );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridEnvironmentLight::SetBrightness( float localTime )
{
	if( _hour < 12 )
	{
		SetZoneBrightness( localTime, _sunriseBrightness, _sunHorizonBrightness, _sunBrightness );
	}
	else
	{
		SetZoneBrightness( localTime, _moonriseBrightess, _moonHorizonBrightness, _moonBrightness );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridEnvironmentLight::SetZoneBrightness( float localTime, const Vector4D &riseBrightess, const Vector4D &horizonBrightess, const Vector4D &brightness )
{
	const Vector4D *start = NULL;
	const Vector4D *end = NULL;
	vec_t percent = 0.0f;

	if( localTime < 0.5f )
	{
		start = &_blankBrightness;
		end = &riseBrightess;
		percent = localTime / 0.5f;
	}
	else if( localTime < 1.0f )
	{
		// Inital 1 hour of sunrise.
		start = &riseBrightess;
		end = &horizonBrightess;
		percent = ( localTime - 0.5f ) / 0.5f;
	}
	else if( localTime < 6.0f )
	{
		start = &horizonBrightess;
		end = &brightness;
		percent = ( localTime - 1.0f ) / 5.0f;
	}
	else if( localTime < 11.0f )
	{
		start = &brightness;
		end = &horizonBrightess;
		percent = ( localTime - 6.0f ) / 5.0f;
	}
	else if( localTime < 11.5 )
	{
		start = &horizonBrightess;
		end = &riseBrightess;
		percent = ( localTime - 11.0f ) / 0.5f;
	}
	else /* if( localTime < 12.0f ) */
	{
		start = &riseBrightess;
		end = &_blankBrightness;
		percent = ( localTime - 11.5f ) / 0.5f;
	}

	Vector4D dest;
	Vector4DLerp( *start, *end, percent, dest );

	m_LightColor.GetForModify(0) = dest[0];
	m_LightColor.GetForModify(1) = dest[1];
	m_LightColor.GetForModify(2) = dest[2];
	m_LightColor.GetForModify(3) = dest[3];
}