/*
===============================================================================

	grid_client.cpp
	Implements server-engine glue.

===============================================================================
*/

#include "cbase.h"
#include "grid_player.h"
#include "game.h"
#include "tier0/vprof.h"

//-----------------------------------------------------------------------------
// Called each time a player is spawned into the game
//-----------------------------------------------------------------------------
void ClientPutInServer( edict_t *pEdict, const char *playername )
{
	// Allocate a CBasePlayer for pev, and call spawn
	CGridPlayer *pPlayer = CGridPlayer::CreatePlayer( "player", pEdict );
	pPlayer->SetPlayerName( playername );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void ClientActive( edict_t *pEdict, bool bLoadGame )
{
	CGridPlayer *pPlayer = dynamic_cast< CGridPlayer* >( CBaseEntity::Instance( pEdict ) );
	Assert( pPlayer );

	if ( !pPlayer )
	{
		return;
	}

	pPlayer->InitialSpawn();

	if ( !bLoadGame )
	{
		pPlayer->Spawn();
	}
}

//-----------------------------------------------------------------------------
// Returns the descriptive name of this .dll.  E.g., Half-Life, or Team Fortress 2
//-----------------------------------------------------------------------------
const char *GetGameDescription()
{
	return "Grid";
}

//-----------------------------------------------------------------------------
// Purpose: Given a player and optional name returns the entity of that 
//			classname that the player is nearest facing
//			
// Input  :
// Output :
//-----------------------------------------------------------------------------
CBaseEntity* FindEntity( edict_t *pEdict, char *classname)
{
	// If no name was given set bits based on the picked
	if (FStrEq(classname,"")) 
	{
		extern CBaseEntity*	FindPickerEntityClass( CBasePlayer *pPlayer, char *classname );
		return (FindPickerEntityClass( static_cast<CBasePlayer*>(GetContainingEntity(pEdict)), classname ));
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Precache game-specific models & sounds
//-----------------------------------------------------------------------------
void ClientGamePrecache( void )
{
	CBaseEntity::PrecacheModel( "models/player.mdl" );
	CBaseEntity::PrecacheModel ("models/weapons/v_hands.mdl");

	CBaseEntity::PrecacheScriptSound( "Bullets.DefaultNearmiss" );	
	CBaseEntity::PrecacheScriptSound( "Geiger.BeepHigh" );
	CBaseEntity::PrecacheScriptSound( "Geiger.BeepLow" );
}

//-----------------------------------------------------------------------------
// Called by ClientKill and DeadThink
//-----------------------------------------------------------------------------
void respawn( CBaseEntity *pEdict, bool fCopyCorpse )
{
	if (gpGlobals->coop || gpGlobals->deathmatch)
	{
		if ( fCopyCorpse )
		{
			// make a copy of the dead body for appearances sake
			((CGridPlayer *)pEdict)->CreateCorpse();
		}

		// respawn player
		pEdict->Spawn();
	}
	else
	{       // restart the entire server
		engine->ServerCommand("reload\n");
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void GameStartFrame( void )
{
	VPROF("GameStartFrame()");
	if ( g_fGameOver )
		return;

	gpGlobals->teamplay = (teamplay.GetInt() != 0);
}

//-----------------------------------------------------------------------------
// Instantiate the proper game rules object
//-----------------------------------------------------------------------------
void InstallGameRules()
{
	CreateGameRulesObject( "CGridGameRules" );
}