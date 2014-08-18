/*
===============================================================================

	grid_gamemovement.cpp
	Mostly borrowed from `hl_gamemovement.cpp`

===============================================================================
*/

#include "cbase.h"
#include "grid_gamemovement.h"
#include "player_command.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Create required objects.
//-----------------------------------------------------------------------------
static CMoveData g_MoveData;
CMoveData *g_pMoveData = &g_MoveData;


IPredictionSystem *IPredictionSystem::g_pPredictionSystems = NULL;


static CPlayerMove g_PlayerMove;
CPlayerMove *PlayerMove()
{
	return &g_PlayerMove;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGridGameMovement::CGridGameMovement()
{
}

// Expose our interface.
static CGridGameMovement g_GameMovement;
IGameMovement *g_pGameMovement = ( IGameMovement * )&g_GameMovement;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGameMovement, IGameMovement,INTERFACENAME_GAMEMOVEMENT, g_GameMovement );