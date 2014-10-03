/*
===============================================================================

	grid_environment_light.cpp
	Implements the actual rendering of the sun\moon lighting system for Grid.

===============================================================================
*/

#include "cbase.h"
#include "c_sunlightshadowcontrol.h"

ConVarRef mat_ambient_light_r( "mat_ambient_light_r", false );
ConVarRef mat_ambient_light_g( "mat_ambient_light_g", false );
ConVarRef mat_ambient_light_b( "mat_ambient_light_b", false );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridEnvironmentLight : public C_SunlightShadowControl
{
public:
	DECLARE_CLASS( C_GridEnvironmentLight, C_SunlightShadowControl );
	DECLARE_CLIENTCLASS();
	C_GridEnvironmentLight();

	virtual void	OnDataChanged( DataUpdateType_t updateType );

private:
	float			_ambience[4];
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IMPLEMENT_CLIENTCLASS_DT(C_GridEnvironmentLight, DT_GridEnvironmentLight, CGridEnvironmentLight)
	RecvPropArray( RecvPropFloat(RECVINFO(_ambience[0]), 0), _ambience ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
C_GridEnvironmentLight::C_GridEnvironmentLight()
{
	for( int i = 0; i < 4; i++ )
	{
		_ambience[i] = 0.0f;
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_GridEnvironmentLight::OnDataChanged( DataUpdateType_t updateType )
{
	if( !mat_ambient_light_r.IsValid() )
	{
		mat_ambient_light_r.Init( "mat_ambient_light_r", true );
	}
	if( !mat_ambient_light_g.IsValid() )
	{
		mat_ambient_light_g.Init( "mat_ambient_light_g", true );
	}
	if( !mat_ambient_light_b.IsValid() )
	{
		mat_ambient_light_b.Init( "mat_ambient_light_b", true );
	}

	BaseClass::OnDataChanged( updateType );

	mat_ambient_light_r.SetValue( _ambience[0] / 255.0f );
	mat_ambient_light_g.SetValue( _ambience[1] / 255.0f );
	mat_ambient_light_b.SetValue( _ambience[2] / 255.0f );
}