/*
===============================================================================

	grid_sun.cpp
	Extends the base Source Engine sun with real time movement.

===============================================================================
*/

#include "cbase.h"
#include "grid_sun.h"
#include "map_utils.h"

LINK_ENTITY_TO_CLASS( grid_env_sun, CGridSun );

IMPLEMENT_SERVERCLASS_ST( CGridSun, DT_GridSun )
	SendPropBool( SENDINFO(_farZ) ),
END_SEND_TABLE()

BEGIN_DATADESC( CGridSun )

	DEFINE_FIELD( _farZ,		FIELD_BOOLEAN ),

END_DATADESC()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGridSun::CGridSun()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridSun::Activate()
{
	m_bUseAngles = true;

	SetRenderColor( 100, 80, 80 );
	m_flHDRColorScale = 1.0f;

	BaseClass::Activate();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridSun::SetPitchYaw( float pitch, float yaw )
{
	m_flPitch = pitch;
	m_flYaw = yaw;

	// Calculate angles given pitch and yaw.
	SetupLightNormalFromProps( GetAbsAngles(), m_flYaw, m_flPitch, m_vDirection.GetForModify() );
	m_vDirection = -m_vDirection.Get();

	_lightDirection = m_vDirection;

	// Store the sun sprite direction.
	Vector &lightDir = m_vDirection.GetForModify();
	lightDir.x = -m_vDirection.GetX();
	lightDir.y = fabs( m_vDirection.GetY() );
	lightDir.z = fabs( m_vDirection.GetZ() );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridSun::SetFarZ( bool farz )
{
	_farZ = farz;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridSun::SetMaterial( const char *material )
{
	m_strMaterial = AllocPooledString( material );
	m_nMaterial = PrecacheModel( STRING( m_strMaterial ) );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const Vector &CGridSun::GetLightDirection() const
{
	return _lightDirection;
}