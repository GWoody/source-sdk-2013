/*
===============================================================================

	c_grid_player.cpp
	Client side implementation of the Grid player entity.

===============================================================================
*/

#include "cbase.h"
#include "c_baseplayer.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridPlayer : public C_BasePlayer
{
public:
	DECLARE_CLASS( C_GridPlayer, C_BasePlayer );
	DECLARE_CLIENTCLASS();
	
private:
	EHANDLE			m_hHand;
};

LINK_ENTITY_TO_CLASS( player, C_GridPlayer );

// Network table.
IMPLEMENT_CLIENTCLASS_DT( C_GridPlayer, DT_GridPlayer, CGridPlayer )

	RecvPropEHandle	( RECVINFO(m_hHand) ),

END_RECV_TABLE()