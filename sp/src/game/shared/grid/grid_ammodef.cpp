/*
===============================================================================

	grid_ammodef.cpp
	Defines valid ammo types for Grid.

===============================================================================
*/

#include "cbase.h"
#include "ammodef.h"

//-----------------------------------------------------------------------------
// ConVars.
//-----------------------------------------------------------------------------
ConVar	sk_plr_dmg_smallround	( "sk_plr_dmg_pistol", "0", FCVAR_REPLICATED );
ConVar	sk_npc_dmg_smallround	( "sk_npc_dmg_pistol", "0", FCVAR_REPLICATED );
ConVar	sk_max_smallround		( "sk_max_pistol", "9999", FCVAR_REPLICATED );

ConVar	sk_plr_dmg_mediumround	( "sk_plr_dmg_smg1", "0", FCVAR_REPLICATED );
ConVar	sk_npc_dmg_mediumround	( "sk_npc_dmg_smg1", "0", FCVAR_REPLICATED );
ConVar	sk_max_mediumround		( "sk_max_smg1", "9999", FCVAR_REPLICATED );

ConVar	sk_plr_dmg_largeround	( "sk_plr_dmg_357", "0", FCVAR_REPLICATED );
ConVar	sk_npc_dmg_largeround	( "sk_npc_dmg_357", "0", FCVAR_REPLICATED );
ConVar	sk_max_largeround		( "sk_max_357", "9999", FCVAR_REPLICATED );

ConVar	sk_plr_dmg_buckshot		( "sk_plr_dmg_buckshot", "0", FCVAR_REPLICATED );	
ConVar	sk_npc_dmg_buckshot		( "sk_npc_dmg_buckshot", "0", FCVAR_REPLICATED );
ConVar	sk_max_buckshot			( "sk_max_buckshot", "9999", FCVAR_REPLICATED );
ConVar	sk_plr_num_shotgun_pellets( "sk_plr_num_shotgun_pellets", "7", FCVAR_REPLICATED );

ConVar	sk_plr_dmg_grenade		( "sk_plr_dmg_grenade", "0", FCVAR_REPLICATED );
ConVar	sk_npc_dmg_grenade		( "sk_npc_dmg_grenade", "0", FCVAR_REPLICATED );
ConVar	sk_max_grenade			( "sk_max_grenade", "9999", FCVAR_REPLICATED );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// shared ammo definition
// JAY: Trying to make a more physical bullet response
#define BULLET_MASS_GRAINS_TO_LB(grains)	(0.002285*(grains)/16.0f)
#define BULLET_MASS_GRAINS_TO_KG(grains)	lbs2kg(BULLET_MASS_GRAINS_TO_LB(grains))

// exaggerate all of the forces, but use real numbers to keep them consistent
#define BULLET_IMPULSE_EXAGGERATION			3.5
// convert a velocity in ft/sec and a mass in grains to an impulse in kg in/s
#define BULLET_IMPULSE(grains, ftpersec)	((ftpersec)*12*BULLET_MASS_GRAINS_TO_KG(grains)*BULLET_IMPULSE_EXAGGERATION)

CAmmoDef *GetAmmoDef()
{
	static CAmmoDef def;
	static bool bInitted = false;
	
	if ( !bInitted )
	{
		bInitted = true;
		
		def.AddAmmoType( "Pistol",			DMG_BULLET,					TRACER_LINE_AND_WHIZ,	"sk_plr_dmg_smallround",		"sk_npc_dmg_smallround",		"sk_max_smallround",			BULLET_IMPULSE(200, 1225), 0 );
		def.AddAmmoType( "SMG1",			DMG_BULLET,					TRACER_LINE_AND_WHIZ,	"sk_plr_dmg_mediumround",		"sk_npc_dmg_mediumround",		"sk_max_mediumround",			BULLET_IMPULSE(200, 1225), 0 );
		def.AddAmmoType( "357",				DMG_BULLET,					TRACER_LINE_AND_WHIZ,	"sk_plr_dmg_largeround",		"sk_npc_dmg_largeround",		"sk_max_largeround",			BULLET_IMPULSE(800, 5000), 0 );
		def.AddAmmoType( "Buckshot",		DMG_BULLET | DMG_BUCKSHOT,	TRACER_LINE,			"sk_plr_dmg_buckshot",			"sk_npc_dmg_buckshot",			"sk_max_buckshot",				BULLET_IMPULSE(400, 1200), 0 );
		def.AddAmmoType( "Grenade",			DMG_BURN,					TRACER_NONE,			"sk_plr_dmg_grenade",			"sk_npc_dmg_grenade",			"sk_max_grenade",				0, 0 );
	}

	return &def;
}