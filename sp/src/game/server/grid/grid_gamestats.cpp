/*
===============================================================================

	grid_gamestats.cpp
		Registers a gamestats instance.

===============================================================================
*/

#include "cbase.h"
#include "gamestats.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGameStats : public CBaseGameStats
{
public:
	CGameStats( void )
	{
		// Set this game stats instance to be the global stat manager.
		gamestats = &s_GameStats;
	}
		
private:
	static CGameStats s_GameStats;
};

CGameStats CGameStats::s_GameStats;