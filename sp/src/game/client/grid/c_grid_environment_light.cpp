/*
===============================================================================

	grid_environment_light.cpp
	Implements the actual rendering of the sun\moon lighting system for Grid.

===============================================================================
*/

#include "cbase.h"
#include "c_sunlightshadowcontrol.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridEnvironmentLight : public C_SunlightShadowControl
{
public:
	DECLARE_CLASS( C_GridEnvironmentLight, C_SunlightShadowControl );
	DECLARE_CLIENTCLASS();
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IMPLEMENT_CLIENTCLASS_DT(C_GridEnvironmentLight, DT_GridEnvironmentLight, CGridEnvironmentLight)
END_RECV_TABLE()