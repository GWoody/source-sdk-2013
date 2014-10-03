/*
===============================================================================

	grid_environment_light.cpp
	Implements the sun\moon lighting system for Grid.

===============================================================================
*/

#include "cbase.h"
#include "sunlightshadowcontrol.h"
#include "grid_sun.h"

static ConVar grid_sun_speed_multiplier( "grid_sun_speed_multiplier", "1.0f" );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridEnvironmentLight : public CSunlightShadowControl
{
	struct LightLevel_t
	{
		Vector4D	_rise;		// Object is about to come over the horizon.
		Vector4D	_horizon;	// Object is on the horizon.
		Vector4D	_highest;	// Object is directly overhead.
	};

public:
	DECLARE_CLASS( CGridEnvironmentLight, CSunlightShadowControl );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
	CGridEnvironmentLight();

	virtual void	Spawn();
	virtual void	Think();

	void			InputSetMaxPitch( inputdata_t &data );

private:
	void			SetSun();
	void			SetMoon();

	float			CalculatePitch( float localTime );
	void			SetBrightness( float pitch );
	void			SetZoneBrightness( float pitch, const LightLevel_t &light );

	CHandle<CGridSun>	_sunSprite;

	float			_maxPitch;
	float			_hour;

	LightLevel_t	_sunBrightness;
	LightLevel_t	_moonBrightness;

	Vector4D		_blankBrightness;
	Vector4D		_blankAmbient;
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
	_sunBrightness._rise.Init( 127.0f, 0.0f, 0.0f, 1.0f );
	_sunBrightness._horizon.Init( 237.0f, 200.0f, 127.0f, 1.0f );
	_sunBrightness._highest.Init( 237.0f, 218.0f, 143.0f, 3.0f );

	_moonBrightness._rise.Init( 16.0f, 16.0f, 16.0f, 0.1f );
	_moonBrightness._horizon.Init( 32.0f, 32.0f, 32.0f, 0.1f );
	_moonBrightness._highest.Init( 64.0f, 64.0f, 64.0f, 0.15f );

	_blankBrightness.Init( 16.0f, 16.0f, 16.0f, 0.1f );
	_blankAmbient.Init( 0.0f, 0.0f, 0.0f, 0.0f );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridEnvironmentLight::Spawn( void )
{
	BaseClass::Spawn();

	//
	// Precache.
	//
	PrecacheModel( "sky/moon.vmt" );

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
// Assume the night starts at (_hour = 12.00f).
//-----------------------------------------------------------------------------
void CGridEnvironmentLight::Think( void )
{
	_hour += gpGlobals->frametime * grid_sun_speed_multiplier.GetFloat();

	if( _hour >= 24.0f )
	{
		// Reset the day.
		_hour = _hour - 24.0f;
	}

	if( _hour >= 12.0f )
	{
		SetMoon();
	}
	else
	{
		SetSun();
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

		SetBrightness( pitch );
	}

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
//-----------------------------------------------------------------------------
void CGridEnvironmentLight::SetSun()
{
	// Sun is bright. Don't worry about Z ordering.
	_sunSprite->SetFarZ( false );

	_sunSprite->SetMaterial( "sprites/light_glow02_add_noz.vmt" );
	_sunSprite->m_nSize = 48;
	_sunSprite->m_nOverlaySize = 16;

	color32 &overlayClr = _sunSprite->m_clrOverlay.GetForModify();
	overlayClr.r = overlayClr.g = overlayClr.b = 0;
	overlayClr.a = 255;

	color32 &sunClr = _sunSprite->m_clrRender.GetForModify();
	sunClr.r = 100;
	sunClr.g = sunClr.b = 80;
	sunClr.a = 255;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridEnvironmentLight::SetMoon()
{
	// Moon has real detail. Render it in order.
	_sunSprite->SetFarZ( true );

	_sunSprite->SetMaterial( "sky/moon.vmt" );
	_sunSprite->m_nSize = 6;
	_sunSprite->m_nOverlaySize = 6;
	_sunSprite->m_clrOverlay = _sunSprite->m_clrRender;

	color32 &overlayClr = _sunSprite->m_clrOverlay.GetForModify();
	overlayClr.r = overlayClr.g = overlayClr.b = 0;
	overlayClr.a = 255;

	color32 &sunClr = _sunSprite->m_clrRender.GetForModify();
	sunClr.r = sunClr.g = sunClr.b = 255;
	sunClr.a = 255;
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
void CGridEnvironmentLight::SetBrightness( float pitch )
{
	if( _hour < 12 )
	{
		SetZoneBrightness( pitch, _sunBrightness );
	}
	else
	{
		SetZoneBrightness( pitch, _moonBrightness );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridEnvironmentLight::SetZoneBrightness( float pitch, const LightLevel_t &light )
{
	const Vector4D *start = NULL;
	const Vector4D *end = NULL;
	vec_t percent = 0.0f;

	// Sun is on the way up.
	if( pitch < 15.0f )
	{
		start = &_blankBrightness;
		end = &light._rise;
		percent = pitch / 15.0f;
	}
	else if( pitch < 30.0f )
	{
		// Inital 1 hour of sunrise.
		start = &light._rise;
		end = &light._horizon;
		percent = ( pitch - 15.0f ) / 15.0f;
	}
	else /*if( pitch < 90.0f )*/
	{
		start = &light._horizon;
		end = &light._highest;
		percent = ( pitch - 30.0f ) / 60.0f;
	}

	Vector4D dest;
	Vector4DLerp( *start, *end, percent, dest );

	m_LightColor.GetForModify(0) = dest[0];
	m_LightColor.GetForModify(1) = dest[1];
	m_LightColor.GetForModify(2) = dest[2];
	m_LightColor.GetForModify(3) = dest[3];
}