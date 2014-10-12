/*
===============================================================================

	grid_environment_light.cpp
	Implements the sun\moon lighting system for Grid.

===============================================================================
*/

#include "cbase.h"
#include "sunlightshadowcontrol.h"
#include "grid_sun.h"

#include "GameEventListener.h"

ConVar grid_sun_speed_multiplier( "grid_sun_speed_multiplier", "1.0f" );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridEnvironmentLight : public CSunlightShadowControl, public CGameEventListener
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

	// CGameEventListener.
	virtual void	FireGameEvent( IGameEvent *event );

	// Source entity overrides.
	virtual void	Spawn();
	virtual void	Think();

	void			InputSetMaxPitch( inputdata_t &data );

private:
	void			SetSun();
	void			SetMoon();

	float			CalculatePitch( float localTime );
	void			SetLightLevel( float pitch );
	void			SetZoneLightLevel( float pitch, const LightLevel_t &light, float level[4] );

	CHandle<CGridSun>	_sunSprite;

	float			_maxPitch;
	float			_hour;

	LightLevel_t	_sunBrightness;
	LightLevel_t	_sunAmbience;

	LightLevel_t	_moonBrightness;
	LightLevel_t	_moonAmbience;

	Vector4D		_blankBrightness;
	Vector4D		_blankAmbient;

	CNetworkArray( float, _ambience, 4 );

	COutputEvent	_onDayBegin, _onNightBegin;
	bool			_isDayTime;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( grid_environment_light, CGridEnvironmentLight );

IMPLEMENT_SERVERCLASS_ST( CGridEnvironmentLight, DT_GridEnvironmentLight )
	SendPropArray( SendPropFloat( SENDINFO_ARRAY(_ambience) ), _ambience),
END_SEND_TABLE()

BEGIN_DATADESC( CGridEnvironmentLight )

	// Outputs.
	DEFINE_OUTPUT( _onDayBegin, "OnDayBegin" ),
	DEFINE_OUTPUT( _onNightBegin, "OnNightBegin" ),

	// Inputs.
	DEFINE_INPUTFUNC( FIELD_FLOAT, "SetMaxPitch", InputSetMaxPitch ),

END_DATADESC();

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGridEnvironmentLight::CGridEnvironmentLight()
{
	{
		_sunBrightness._rise.Init( 32.0f, 0.0f, 0.0f, 0.25f );
		_sunBrightness._horizon.Init( 237.0f, 200.0f, 127.0f, 1.0f );
		_sunBrightness._highest.Init( 237.0f, 218.0f, 143.0f, 3.0f );

		_sunAmbience._rise.Init( 8.0f, 8.0f, 8.0f, 1.0f );
		_sunAmbience._horizon.Init( 48.0f, 48.0f, 48.0f, 1.0f );
		_sunAmbience._highest.Init( 64.0f, 64.0f, 64.0f, 1.0f );
	}

	{
		_moonBrightness._rise.Init( 16.0f, 16.0f, 16.0f, 0.1f );
		_moonBrightness._horizon.Init( 32.0f, 32.0f, 32.0f, 0.1f );
		_moonBrightness._highest.Init( 64.0f, 64.0f, 64.0f, 0.1f );

		_moonAmbience._rise.Init( 0.0f, 0.0f, 0.0f, 1.0f );
		_moonAmbience._horizon.Init( 4.0f, 4.0f, 4.0f, 1.0f );
		_moonAmbience._highest.Init( 8.0f, 8.0f, 8.0f, 1.0f );
	}

	_blankBrightness.Init( 16.0f, 16.0f, 16.0f, 0.1f );
	_blankAmbient.Init( 0.0f, 0.0f, 0.0f, 0.0f );

	for( int i = 0; i < 4; i++ )
	{
		_ambience.GetForModify( i ) = 0.0f;
	}

	ListenForGameEvent( "grid_env_light_color" );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridEnvironmentLight::FireGameEvent( IGameEvent *event )
{
	bool isSun = event->GetBool( "issun", true );
	bool isAmbient = event->GetBool( "isambient", false );
	float r = event->GetFloat( "r" );
	float g = event->GetFloat( "g" );
	float b = event->GetFloat( "b" );
	float a = event->GetFloat( "a" );

	if( isSun )
	{
		if( isAmbient )
		{
			_sunAmbience._highest.Init( r, g, b, a );	
		}
		else
		{
			_sunBrightness._highest.Init( r, g, b, a );
		}
	}
	else
	{
		if( isAmbient )
		{
			_moonAmbience._highest.Init( r, g, b, a );	
		}
		else
		{
			_moonBrightness._highest.Init( r, g, b, a );
		}
	}
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
	_isDayTime = true;
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

	//
	// Ensure the time is kept within a valid range.
	//
	{
		while( _hour >= 24.0f )
		{
			_hour = _hour - 24.0f;
		}
		while( _hour < 0.0f )
		{
			_hour = _hour + 24.0f;
		}
	}

	//
	// Fire map events
	//
	{
		if( _isDayTime && _hour >= 12.0f )
		{
			_onNightBegin.FireOutput( this, this );
		}
		else if( !_isDayTime && _hour < 12 )
		{
			_onDayBegin.FireOutput( this, this );
		}
	}

	//
	// Configure the sun\moon sprite.
	//
	{
		if( _hour >= 12.0f )
		{
			SetMoon();
			_isDayTime = false;
		}
		else
		{
			SetSun();
			_isDayTime = true;
		}
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

		SetLightLevel( pitch );
	}

	BaseClass::Think();
	SetNextThink( gpGlobals->curtime + 0.1f );	

	grid_sun_speed_multiplier.SetValue( 0.1f );
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
void CGridEnvironmentLight::SetLightLevel( float pitch )
{
	if( _hour < 12 )
	{
		SetZoneLightLevel( pitch, _sunBrightness, (float *)m_LightColor.Base() );
		SetZoneLightLevel( pitch, _sunAmbience, (float *)_ambience.Base() );
	}
	else
	{
		SetZoneLightLevel( pitch, _moonBrightness, (float *)m_LightColor.Base() );
		SetZoneLightLevel( pitch, _moonAmbience, (float *)_ambience.Base() );
	}

	NetworkStateChanged();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridEnvironmentLight::SetZoneLightLevel( float pitch, const LightLevel_t &light, float level[4] )
{
	const Vector4D *start = NULL;
	const Vector4D *end = NULL;
	vec_t percent = 0.0f;

	// Sun is on the way up.
	if( pitch < 10.0f )
	{
		start = &_blankBrightness;
		end = &light._rise;
		percent = pitch / 10.0f;
	}
	else if( pitch < 30.0f )
	{
		// Inital 1 hour of sunrise.
		start = &light._rise;
		end = &light._horizon;
		percent = ( pitch - 10.0f ) / 20.0f;
	}
	else /*if( pitch < 90.0f )*/
	{
		start = &light._horizon;
		end = &light._highest;
		percent = ( pitch - 30.0f ) / 60.0f;
	}

	Vector4D dest;
	Vector4DLerp( *start, *end, percent, dest );

	level[0] = dest[0];
	level[1] = dest[1];
	level[2] = dest[2];
	level[3] = dest[3];
}