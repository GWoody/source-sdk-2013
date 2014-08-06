/*
===============================================================================

	c_holo_swipe_panel.cpp
		Implements client side rendering of the `holo_swipe_panel` entity.

===============================================================================
*/

#include "cbase.h"
#include "c_base_holo_panel.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_HoloSwipePanel : public C_BaseHoloPanel
{
public:
	DECLARE_CLASS( C_HoloSwipePanel, C_BaseHoloPanel );
	DECLARE_CLIENTCLASS();
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IMPLEMENT_CLIENTCLASS_DT( C_HoloSwipePanel, DT_HoloSwipePanel, CHoloSwipePanel )
END_RECV_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( holo_swipe_panel, C_HoloSwipePanel );