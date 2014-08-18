/*
===============================================================================

	grid_playermove.cpp
	Mostly borrowed from `hl_playermove.cpp`

===============================================================================
*/

#include "cbase.h"
#include "grid_gamemovement.h"
#include "player_command.h"

static CPlayerMove g_PlayerMove;
CPlayerMove *PlayerMove()
{
	return &g_PlayerMove;
}