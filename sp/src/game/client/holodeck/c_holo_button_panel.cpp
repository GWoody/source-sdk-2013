/*
===============================================================================

	c_holo_button_panel.cpp
		Implements client side rendering of the `holo_button_panel` entity.

===============================================================================
*/

#include "cbase.h"
#include "c_holo_base_panel.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_HoloButtonPanel : public C_BaseHoloPanel
{
public:
	DECLARE_CLASS( C_HoloButtonPanel, C_BaseHoloPanel );
	DECLARE_CLIENTCLASS();
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IMPLEMENT_CLIENTCLASS_DT( C_HoloButtonPanel, DT_HoloButtonPanel, CHoloButtonPanel )
END_RECV_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( holo_button_panel, C_HoloButtonPanel );