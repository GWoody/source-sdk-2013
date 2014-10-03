//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Sunlight shadow control entity.
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "sunlightshadowcontrol.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS(sunlight_shadow_control, CSunlightShadowControl);

BEGIN_DATADESC( CSunlightShadowControl )

	DEFINE_KEYFIELD( m_bEnabled,		FIELD_BOOLEAN, "enabled" ),
	DEFINE_KEYFIELD( m_bStartDisabled,	FIELD_BOOLEAN, "StartDisabled" ),
	DEFINE_AUTO_ARRAY_KEYFIELD( m_TextureName, FIELD_CHARACTER, "texturename" ),
	DEFINE_KEYFIELD( m_flSunDistance,	FIELD_FLOAT, "distance" ),
	DEFINE_KEYFIELD( m_flFOV,	FIELD_FLOAT, "fov" ),
	DEFINE_KEYFIELD( m_flNearZ,	FIELD_FLOAT, "nearz" ),
	DEFINE_KEYFIELD( m_flNorthOffset,	FIELD_FLOAT, "northoffset" ),
	DEFINE_KEYFIELD( m_bEnableShadows, FIELD_BOOLEAN, "enableshadows" ),
	DEFINE_AUTO_ARRAY( m_LightColor, FIELD_FLOAT ), 
	DEFINE_KEYFIELD( m_flColorTransitionTime, FIELD_FLOAT, "colortransitiontime" ),

	// Inputs
	DEFINE_INPUT( m_flSunDistance,		FIELD_FLOAT, "SetDistance" ),
	DEFINE_INPUT( m_flFOV,				FIELD_FLOAT, "SetFOV" ),
	DEFINE_INPUT( m_flNearZ,			FIELD_FLOAT, "SetNearZDistance" ),
	DEFINE_INPUT( m_flNorthOffset,			FIELD_FLOAT, "SetNorthOffset" ),

	DEFINE_INPUTFUNC( FIELD_VECTOR, "LightColor", InputSetLightColor ),
	DEFINE_INPUTFUNC( FIELD_STRING, "SetAngles", InputSetAngles ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
	DEFINE_INPUTFUNC( FIELD_STRING, "SetTexture", InputSetTexture ),
	DEFINE_INPUTFUNC( FIELD_BOOLEAN, "EnableShadows", InputSetEnableShadows ),

END_DATADESC()


IMPLEMENT_SERVERCLASS_ST_NOBASE(CSunlightShadowControl, DT_SunlightShadowControl)
	SendPropVector(SENDINFO(m_shadowDirection), -1,  SPROP_NOSCALE ),
	SendPropBool(SENDINFO(m_bEnabled) ),
	SendPropString(SENDINFO(m_TextureName)),
	SendPropArray( SendPropFloat( SENDINFO_ARRAY(m_LightColor) ), m_LightColor),
	SendPropFloat( SENDINFO( m_flColorTransitionTime ) ),
	SendPropFloat(SENDINFO(m_flSunDistance), 0, SPROP_NOSCALE ),
	SendPropFloat(SENDINFO(m_flFOV), 0, SPROP_NOSCALE ),
	SendPropFloat(SENDINFO(m_flNearZ), 0, SPROP_NOSCALE ),
	SendPropFloat(SENDINFO(m_flNorthOffset), 0, SPROP_NOSCALE ),
	SendPropBool( SENDINFO( m_bEnableShadows ) ),
END_SEND_TABLE()


CSunlightShadowControl::CSunlightShadowControl()
{
#if defined( _X360 )
	Q_strcpy( m_TextureName.GetForModify(), "effects/flashlight_border" );
#else
	Q_strcpy( m_TextureName.GetForModify(), "effects/flashlight001" );
#endif
	m_LightColor.GetForModify( 0 ) = m_LightColor.GetForModify( 1 ) = m_LightColor.GetForModify( 2 ) = 255.0f;
	m_LightColor.GetForModify( 3 ) = 1.0f;
	m_flColorTransitionTime = 0.5f;
	m_flSunDistance = 10000.0f;
	m_flFOV = 5.0f;
	m_bEnableShadows = false;
}


//------------------------------------------------------------------------------
// Purpose : Send even though we don't have a model
//------------------------------------------------------------------------------
int CSunlightShadowControl::UpdateTransmitState()
{
	// ALWAYS transmit to all clients.
	return SetTransmitState( FL_EDICT_ALWAYS );
}


bool CSunlightShadowControl::KeyValue( const char *szKeyName, const char *szValue )
{
	if ( FStrEq( szKeyName, "color" ) )
	{
		float tmp[4];
		UTIL_StringToFloatArray( tmp, 4, szValue );

		m_LightColor.GetForModify( 0 ) = tmp[0];
		m_LightColor.GetForModify( 1 ) = tmp[1];
		m_LightColor.GetForModify( 2 ) = tmp[1];
		m_LightColor.GetForModify( 3 ) = tmp[3];
	}
	else if ( FStrEq( szKeyName, "angles" ) )
	{
		QAngle angles;
		UTIL_StringToVector( angles.Base(), szValue );
		if (angles == vec3_angle)
		{
			angles.Init( 80, 30, 0 );
		}
		Vector vForward;
		AngleVectors( angles, &vForward );
		m_shadowDirection = vForward;
		return true;
	}
	else if ( FStrEq( szKeyName, "texturename" ) )
	{
#if defined( _X360 )
		if ( Q_strcmp( szValue, "effects/flashlight001" ) == 0 )
		{
			// Use this as the default for Xbox
			Q_strcpy( m_TextureName.GetForModify(), "effects/flashlight_border" );
		}
		else
		{
			Q_strcpy( m_TextureName.GetForModify(), szValue );
		}
#else
		Q_strcpy( m_TextureName.GetForModify(), szValue );
#endif
	}

	return BaseClass::KeyValue( szKeyName, szValue );
}

bool CSunlightShadowControl::GetKeyValue( const char *szKeyName, char *szValue, int iMaxLen )
{
	if ( FStrEq( szKeyName, "color" ) )
	{
		Q_snprintf( szValue, iMaxLen, "%d %d %d %d", (int)m_LightColor.Get(0), (int)m_LightColor.Get(1), (int)m_LightColor.Get(2), (int)m_LightColor.Get(3) );
		return true;
	}
	else if ( FStrEq( szKeyName, "texturename" ) )
	{
		Q_snprintf( szValue, iMaxLen, "%s", m_TextureName.Get() );
		return true;
	}
	return BaseClass::GetKeyValue( szKeyName, szValue, iMaxLen );
}

//------------------------------------------------------------------------------
// Purpose :
//------------------------------------------------------------------------------
void CSunlightShadowControl::Spawn( void )
{
	Precache();
	SetSolid( SOLID_NONE );

	if( m_bStartDisabled )
	{
		m_bEnabled = false;
	}
	else
	{
		m_bEnabled = true;
	}
}

//------------------------------------------------------------------------------
// Input values
//------------------------------------------------------------------------------
void CSunlightShadowControl::InputSetAngles( inputdata_t &inputdata )
{
	const char *pAngles = inputdata.value.String();

	QAngle angles;
	UTIL_StringToVector( angles.Base(), pAngles );

	Vector vTemp;
	AngleVectors( angles, &vTemp );
	m_shadowDirection = vTemp;
}

//------------------------------------------------------------------------------
// Purpose : Input handlers
//------------------------------------------------------------------------------
void CSunlightShadowControl::InputEnable( inputdata_t &inputdata )
{
	m_bEnabled = true;
}

void CSunlightShadowControl::InputDisable( inputdata_t &inputdata )
{
	m_bEnabled = false;
}

void CSunlightShadowControl::InputSetTexture( inputdata_t &inputdata )
{
	Q_strcpy( m_TextureName.GetForModify(), inputdata.value.String() );
}

void CSunlightShadowControl::InputSetEnableShadows( inputdata_t &inputdata )
{
	m_bEnableShadows = inputdata.value.Bool();
}

void CSunlightShadowControl::InputSetLightColor( inputdata_t &inputdata )
{
	color32 color = inputdata.value.Color32();
	m_LightColor.GetForModify(0) = color.r;
	m_LightColor.GetForModify(1) = color.g;
	m_LightColor.GetForModify(2) = color.b;
	m_LightColor.GetForModify(3) = color.a;
}
