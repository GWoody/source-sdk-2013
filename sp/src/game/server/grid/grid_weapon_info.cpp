/*
===============================================================================

	grid_weapon_info.h
	Defines the Grid classes that load weapon scripts.

===============================================================================
*/

#include "cbase.h"
#include "grid_weapon_info.h"
#include "filesystem.h"

using namespace grid;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CBulletInfo::CBulletInfo()
{
	_count = _damage = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CBulletInfo::Parse( KeyValues *kv )
{
	_count = kv->GetInt( "count" );
	_damage = kv->GetInt( "damage" );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CShootInfo::CShootInfo()
{
	_semiauto = false;
	_ejectShell = false;
	_muzzleparticle = NULL_STRING;
	_rate = 0.1f;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CShootInfo::Parse( KeyValues *kv )
{
	_semiauto = kv->GetBool( "semiauto" );
	_ejectShell = kv->GetBool( "ejectshell" );
	_muzzleparticle = AllocPooledString( kv->GetString( "muzzleparticle" ) );
	_rate = kv->GetFloat( "rate" );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CModelInfo::CModelInfo()
{
	_weapon = _shell = NULL_STRING;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CModelInfo::Parse( KeyValues *kv )
{
	UTIL_StringToVector( _angle.Base(), kv->GetString( "angle" ) );
	_weapon = AllocPooledString( kv->GetString( "weapon" ) );
	_shell = AllocPooledString( kv->GetString( "shell" ) );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CHudInfo::CHudInfo()
{
	_font = NULL_STRING;
	_character = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHudInfo::Parse( KeyValues *kv )
{
	_font = AllocPooledString( kv->GetString( "font" ) );
	_character = kv->GetString( "character", "a" )[0];
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CSoundInfo::CSoundInfo()
{
	_shoot = _empty = NULL_STRING;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CSoundInfo::Parse( KeyValues *kv )
{
	_shoot = AllocPooledString( kv->GetString( "shoot" ) );
	_empty = AllocPooledString( kv->GetString( "empty" ) );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CHapticInfo::CHapticInfo()
{
	_power = _freq = 0;
	_duration = 0.0f;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHapticInfo::Parse( KeyValues *kv )
{
	_power = kv->GetInt( "power" );
	_freq = kv->GetInt( "freq" );
	_duration = kv->GetInt( "duration" );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CWeaponInfo::CWeaponInfo()
{
	
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CWeaponInfo::Parse( const char *scriptpath )
{
	KeyValues *kv = new KeyValues( "weapon" );
	if( !kv->LoadFromFile( g_pFullFileSystem, scriptpath ) )
	{
		Warning( "CWeaponInfo::Parse failed to open script \"%s\"\n", scriptpath );
		return;
	}

	// Load all info blocks.
	for( KeyValues *sub = kv->GetFirstTrueSubKey(); sub != NULL; sub = sub->GetNextTrueSubKey() )
	{
		if( !Q_stricmp( sub->GetName(), "bullet" ) )
		{
			_bullet.Parse( sub );
		}
		else if( !Q_stricmp( sub->GetName(), "shoot" ) )
		{
			_shoot.Parse( sub );
		}
		else if( !Q_stricmp( sub->GetName(), "model" ) )
		{
			_model.Parse( sub );
		}
		else if( !Q_stricmp( sub->GetName(), "hud" ) )
		{
			_hud.Parse( sub );
		}
		else if( !Q_stricmp( sub->GetName(), "sound" ) )
		{
			_sound.Parse( sub );
		}
		else if( !Q_stricmp( sub->GetName(), "haptic" ) )
		{
			_haptic.Parse( sub );
		}
	}

	kv->deleteThis();
}