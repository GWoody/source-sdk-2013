/*
===============================================================================

	c_holo_circle_panel.cpp
		Implements client side rendering of the `holo_circle_panel` entity.

===============================================================================
*/

#include "cbase.h"
#include "c_base_holo_panel.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_HoloCirclePanel : public C_BaseHoloPanel
{
public:
	DECLARE_CLASS( C_HoloCirclePanel, C_BaseHoloPanel );
	DECLARE_CLIENTCLASS();
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IMPLEMENT_CLIENTCLASS_DT( C_HoloCirclePanel, DT_HoloCirclePanel, CHoloCirclePanel )
END_RECV_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( holo_circle_panel, C_HoloCirclePanel );