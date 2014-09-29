/*
===============================================================================

	grid_environment_light.cpp
	Implements the sun\moon lighting system for Grid.

===============================================================================
*/

#include "cbase.h"
#include "sunlightshadowcontrol.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridEnvironmentLight : public CSunlightShadowControl
{
public:
	DECLARE_CLASS( CGridEnvironmentLight, CSunlightShadowControl );
	DECLARE_SERVERCLASS();

	virtual void	Spawn();
	virtual void	Think();

private:
	QAngle			_currentAngles;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( grid_environment_light, CGridEnvironmentLight );

IMPLEMENT_SERVERCLASS_ST( CGridEnvironmentLight, DT_GridEnvironmentLight )
END_SEND_TABLE()


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridEnvironmentLight::Spawn( void )
{
	_currentAngles.Init( 0, -30, 0 );

	BaseClass::Spawn();

	m_flSunDistance = 29000.0f;
	m_flFOV = 10.0f;
	m_flNearZ = 2048.0f;
	m_flNorthOffset = 512.0f;
	m_bEnableShadows = true;

	SetNextThink( gpGlobals->curtime + 0.1f );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridEnvironmentLight::Think( void )
{
	_currentAngles.x += gpGlobals->frametime * 30.0f;
	
	const Vector dawn( 255, 0, 0 );
	const Vector midday( 237, 218, 143 );

	if( _currentAngles.x > 180 )
	{
		_currentAngles.x = 0;
	}
	else if( _currentAngles.x > 90 )
	{
		Vector cur = VectorLerp( midday, dawn, (_currentAngles.x - 90) / 90 );
		m_LightColor.SetR( cur.x );
		m_LightColor.SetG( cur.y );
		m_LightColor.SetB( cur.z );
		m_LightColor.SetA( 3 - (_currentAngles.x - 90) / 90 );
	}
	else
	{
		Vector cur = VectorLerp( dawn, midday, _currentAngles.x / 90 );
		m_LightColor.SetR( cur.x );
		m_LightColor.SetG( cur.y );
		m_LightColor.SetB( cur.z );
		m_LightColor.SetA( 2 + _currentAngles.x / 90 );
	}

	Vector vTemp;
	AngleVectors( _currentAngles, &vTemp );
	m_shadowDirection = vTemp;

	BaseClass::Think();
	SetNextThink( gpGlobals->curtime + 0.1f );
}