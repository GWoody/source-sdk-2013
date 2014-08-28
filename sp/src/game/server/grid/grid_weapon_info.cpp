/*
===============================================================================

	grid_weapon_info.h
	Defines the Grid classes that load weapon scripts.

===============================================================================
*/

#include "cbase.h"
#include "grid_weapon_info.h"
#include "filesystem.h"
#include "ammodef.h"

using namespace grid;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CBulletInfo::CBulletInfo()
{
	_count = _ammotype = _maxDistance = _damage = 0;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CBulletInfo::Parse( KeyValues *kv )
{
	_count = kv->GetInt( "count" );
	_maxDistance = kv->GetInt( "maxdistance", 8192 );
	_damage = kv->GetInt( "damage", 0 );
	_ammotype = GetAmmoDef()->Index( kv->GetString( "ammotype", "Default" ) );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CShootInfo::CShootInfo()
{
	_semiauto = false;
	_rate = 0.1f;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CShootInfo::Parse( KeyValues *kv )
{
	_semiauto = kv->GetBool( "semiauto" );
	_rate = kv->GetFloat( "rate", 0.1f );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CEffectInfo::CEffectInfo()
{
	_shellType = -1;
	_shellattachment = _muzzleattachment = _muzzleparticle = _laserPath = NULL_STRING;
	_laserScale = 0.25;
	_laserActive = true;
	
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CEffectInfo::Parse( KeyValues *kv )
{
	_shellType = kv->GetInt( "shelltype", -1 );
	_shellattachment = AllocPooledString( kv->GetString( "shellattachment" ) );
	_muzzleparticle = AllocPooledString( kv->GetString( "muzzleparticle" ) );
	_muzzleattachment = AllocPooledString( kv->GetString( "muzzleattachment" ) );
	_laserPath = AllocPooledString(kv->GetString("path", "sprites/redglow1.vmt"));
	_laserActive = kv->GetBool("active", true);
	_laserScale = kv->GetFloat("scale", 0.25);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CModelInfo::CModelInfo()
{
	_playermodel = _worldmodel = NULL_STRING;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CModelInfo::Parse( KeyValues *kv )
{
	UTIL_StringToVector( _angle.Base(), kv->GetString( "angle" ) );
	_playermodel = AllocPooledString( kv->GetString( "player" ) );
	_worldmodel = AllocPooledString( kv->GetString( "world" ) );
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
	_ammoattachment = AllocPooledString( kv->GetString( "ammoattachment" ) );
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
		else if( !Q_stricmp( sub->GetName(), "effect" ) )
		{
			_effect.Parse( sub );
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