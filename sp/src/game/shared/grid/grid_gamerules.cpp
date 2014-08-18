/*
===============================================================================

	grid_gamerules.cpp
	Defines the game conditions for Grid.

===============================================================================
*/

#include "cbase.h"
#include "grid_gamerules.h"
#include "voice_gamemgr.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CVoiceGameMgrHelper : public IVoiceGameMgrHelper
{
public:
	virtual bool		CanPlayerHearPlayer( CBasePlayer *pListener, CBasePlayer *pTalker, bool &bProximity )
	{
		return true;
	}
};

CVoiceGameMgrHelper g_VoiceGameMgrHelper;
IVoiceGameMgrHelper *g_pVoiceGameMgrHelper = &g_VoiceGameMgrHelper;

//-----------------------------------------------------------------------------
// Inform the engine of this gamerules object.
//-----------------------------------------------------------------------------
REGISTER_GAMERULES_CLASS( CGridRules );

LINK_ENTITY_TO_CLASS( grid_gamerules, CGridProxy );
IMPLEMENT_NETWORKCLASS_ALIASED( GridProxy, DT_GridProxy )

//-----------------------------------------------------------------------------
// Define server->client networking.
//-----------------------------------------------------------------------------
BEGIN_NETWORK_TABLE_NOBASE( CGridRules, DT_GridGameRules )
	#ifdef CLIENT_DLL
		//Recv
	#else
		//Send
	#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
	void RecvProxy_GridGameRules( const RecvProp *pProp, void **pOut, void *pData, int objectID )
	{
		CHalfLife2 *pRules = HL2GameRules();
		Assert( pRules );
		*pOut = pRules;
	}

	BEGIN_RECV_TABLE( CGridProxy, DT_GridProxy )
		RecvPropDataTable( "grid_gamerules_data", 0, 0, &REFERENCE_RECV_TABLE( DT_GridGameRules ), RecvProxy_GridGameRules )
	END_RECV_TABLE()
#else
	void* SendProxy_GridGameRules( const SendProp *pProp, const void *pStructBase, const void *pData, CSendProxyRecipients *pRecipients, int objectID )
	{
		CGridRules *pRules = GridRules();
		Assert( pRules );
		pRecipients->SetAllRecipients();
		return pRules;
	}

	BEGIN_SEND_TABLE( CGridProxy, DT_GridProxy )
		SendPropDataTable( "grid_gamerules_data", 0, &REFERENCE_SEND_TABLE( DT_GridGameRules ), SendProxy_GridGameRules )
	END_SEND_TABLE()
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridRules::PlayerThink( CBasePlayer *pPlayer )
{
}