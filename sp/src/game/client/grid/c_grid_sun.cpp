/*
===============================================================================

	c_grid_sun.cpp
	Extends the base Source Engine sun with real time movement.

===============================================================================
*/

#include "cbase.h"
#include "c_sun.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridSun : public C_Sun
{
public:
	DECLARE_CLASS( C_GridSun, C_Sun );
	DECLARE_CLIENTCLASS();

	virtual void	OnDataChanged( DataUpdateType_t updateType );

private:
	bool			_farZ;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IMPLEMENT_CLIENTCLASS_DT( C_GridSun, DT_GridSun, CGridSun )
	
	RecvPropBool( RECVINFO(_farZ) ),
	
END_RECV_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_GridSun::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	m_Overlay.SetFarZ( _farZ );
}