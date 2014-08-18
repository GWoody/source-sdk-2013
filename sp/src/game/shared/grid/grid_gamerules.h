/*
===============================================================================

	grid_gamerules.h
	Defines the game conditions for Grid.

===============================================================================
*/

#ifndef __GRID_GAMERULES_H__
#define __GRID_GAMERULES_H__

#include "singleplay_gamerules.h"

#ifdef CLIENT_DLL
	#define CGridProxy C_GridProxy
	#define CGridRules C_GridRules
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridProxy : public CGameRulesProxy
{
public:
	DECLARE_CLASS( CGridProxy, CGameRulesProxy );
	DECLARE_NETWORKCLASS();
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridRules : public CSingleplayRules
{
public:
	DECLARE_CLASS( CGridRules, CSingleplayRules );
	
#ifdef CLIENT_DLL

	DECLARE_CLIENTCLASS_NOBASE(); // This makes datatables able to access our private vars.

#else

	DECLARE_SERVERCLASS_NOBASE(); // This makes datatables able to access our private vars.
	
	// CGameRules implementation.
	virtual void	PlayerThink( CBasePlayer *pPlayer );

#endif
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline CGridRules *GridRules()
{
	return static_cast<CGridRules *>( g_pGameRules );
}

#endif // __GRID_GAMERULES_H__