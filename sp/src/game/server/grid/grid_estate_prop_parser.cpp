/*
===============================================================================

	c_grid_estate_prop_parser.cpp
	Creates screenshots of all props in the estate demo.

===============================================================================
*/

#include "cbase.h"
#include "filesystem.h"

void Grid_ParsePropFile( const char *script )
{
	KeyValues *kv = new KeyValues( "props" );
	if( !kv->LoadFromFile( g_pFullFileSystem, script, "MOD" ) )
	{
		Assert( 0 );
		ConColorMsg( COLOR_YELLOW, __FUNCTION__": failed to open \"%s\"\n", script );
		return;
	}

	FOR_EACH_TRUE_SUBKEY( kv, sub )
	{
		CBaseEntity::PrecacheModel( sub->GetName() );
	}

	kv->deleteThis();
}

void Grid_PrecacheEstateProps()
{
	KeyValues *kv = new KeyValues( "prop_manifest" );
	if( !kv->LoadFromFile( g_pFullFileSystem, "scripts/grid_prop_manifest.txt", "MOD" ) )
	{
		Assert( 0 );
		ConColorMsg( COLOR_YELLOW, __FUNCTION__": failed to open \"%s\"\n", "scripts/grid_prop_manifest.txt" );
		return;
	}

	for ( KeyValues *sub = kv->GetFirstValue(); sub; sub = sub->GetNextValue() )
	{
		if( !Q_stricmp( sub->GetName(), "file" ) )
		{
			Grid_ParsePropFile( sub->GetString() );
		}
	}

	kv->deleteThis();
}