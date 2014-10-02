/*
===============================================================================

	grid_material_patch.cpp
	Fixes materials referenced by the BSP to use custom shader types.

===============================================================================
*/

#include "cbase.h"
#include "grid_utils.h"
#include "model_types.h"
#include "filesystem.h"

#include "materialsystem/imaterial.h"
#include "materialsystem/imaterialvar.h"
#include "materialsystem/itexture.h"
#include "iclientrenderable.h"

//-----------------------------------------------------------------------------
// Prototypes.
//-----------------------------------------------------------------------------
static bool string_t_CmpFunc( const string_t &lhs, const string_t &rhs );
static void Grid_RunStudioShaderReplacement( const model_t *model );
static void Grid_RunBrushShaderReplacement( const model_t *model, bool worldmodel );
static void Grid_PerformReplacement( IMaterial *material );

static KeyValues *Grid_MaterialParamsToKeyValues( const char *shader, IMaterialVar **vars, int count );
static int Grid_MaterialFlagsToInt( IMaterial *material );
static void Grid_IntToMaterialFlags( int flags, IMaterial *material );

//-----------------------------------------------------------------------------
// Variables.
//-----------------------------------------------------------------------------
bool gShaderReplacementTableInitialized = false;
CUtlMap<string_t, string_t> gShaderReplacementTable( string_t_CmpFunc );
CUtlMap<string_t, bool> gModifiedMaterials( string_t_CmpFunc );
CUtlMap<string_t, bool> gBitchTable( string_t_CmpFunc );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static bool string_t_CmpFunc( const string_t &lhs, const string_t &rhs )
{ 
	return CaselessStringLessThan( STRING(lhs), STRING(rhs) ); 
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void Grid_CreateShaderReplacementPair( const char *find, const char *replace )
{
	string_t strFind = AllocPooledString( find );
	string_t strReplace = AllocPooledString( replace );

	gShaderReplacementTable.Insert( strFind, strReplace );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void Grid_InitShaderReplacementTable()
{
	if( gShaderReplacementTableInitialized )
	{
		return;
	}

	Grid_CreateShaderReplacementPair( "LightmappedGeneric", "SDK_LightmappedGeneric" );
	Grid_CreateShaderReplacementPair( "VertexLitGeneric", "SDK_VertexLitGeneric" );

	gShaderReplacementTableInitialized = true;
}

//-----------------------------------------------------------------------------
// Makes all materials used by the given model reference custom shaders instead.
//-----------------------------------------------------------------------------
void Grid_RunShaderReplacement( const char *path )
{
	Grid_InitShaderReplacementTable();

	// Load the model model.
	int idx = engine->PrecacheModel( path, true );
	const model_t *model = modelinfo->GetModel( idx );
	
	switch( modelinfo->GetModelType( model ) )
	{
		case mod_studio:
			Grid_RunStudioShaderReplacement( model );
			break;

		case mod_brush:
			Grid_RunBrushShaderReplacement( model, true );
			break;

		default:
			break;
	}
}

//-----------------------------------------------------------------------------
// Forgets all materials that have been modified.
//-----------------------------------------------------------------------------
void Grid_ClearShaderReplacementList()
{
	gModifiedMaterials.RemoveAll();
	gBitchTable.RemoveAll();
}

//-----------------------------------------------------------------------------
// For some reason `modelinfo->GetModelMaterialCount` is 0 for studio models.
// We must therefore load and reference the textures for studio models manually.
//-----------------------------------------------------------------------------
static void Grid_RunStudioShaderReplacement( const model_t *model )
{
	studiohdr_t *studio = modelinfo->GetStudiomodel( model );
	if( !studio )
	{
		return;
	}

	// Load all textures.
	for( int i = 0; i < studio->numtextures; i++ )
	{
		bool found = false;

		// Search for the texture in all the models search paths.
		mstudiotexture_t *texture = studio->pTexture( i );
		for( int j = 0; j < studio->numcdtextures; j++ )
		{
			const char *cd = studio->pCdtexture( j );
			char cdtex[260];

			// Build the current path.
			Q_snprintf( cdtex, sizeof(cdtex), "%s%s.vmt", cd, texture->pszName() );

			// Use `PrecacheModel` because `PrecacheMaterial` only adds the given
			// path to a string table, whereas `PrecacheModel` actually loads the material.
			int idx = engine->PrecacheModel( cdtex );
			if( idx != -1 )
			{
				IMaterial *material = materials->FindMaterial( cdtex, TEXTURE_GROUP_OTHER, false );
				Grid_PerformReplacement( material );
				found = true;
			}

			if( found )
			{
				// We found the material in a search path. We can now move into the next material.
				break;
			}
		}

		if( !found && gBitchTable.Find( AllocPooledString( texture->pszName() ) ) == gBitchTable.InvalidIndex() )
		{
			// This should only break if there is something wrong in the model.
			Assert( 0 );

			ConColorMsg( COLOR_BLUE, "Failed to find material %s for patching!\n", texture->pszName() );
			gBitchTable.Insert( AllocPooledString( texture->pszName() ), true );
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void Grid_RunBrushShaderReplacement( const model_t *model, bool worldmodel )
{
	// Get the materials it references.
	int materialCount = modelinfo->GetModelMaterialCount( model );
	if( !materialCount )
	{
		return;
	}

	//
	// Do BSP model.
	//
	{
		IMaterial **materials = new IMaterial *[materialCount];
		modelinfo->GetModelMaterials( model, materialCount, materials );

		for( int i = 0; i < materialCount; i++ )
		{
			IMaterial *material = materials[i];

			if( !material )
			{
				continue;
			}

			Grid_PerformReplacement( material );
		}

		delete materials;
	}

	//
	// Do associated static props.
	//
	if( worldmodel )
	{
		CUtlVector<ICollideable *> props;
		staticpropmgr->GetAllStaticProps( &props );

		for( int i = 0; i < props.Count(); i++ )
		{
			IClientRenderable *renderable = props[i]->GetIClientUnknown()->GetClientRenderable();
			if( !renderable )
			{
				continue;
			}

			const model_t *model = renderable->GetModel();

			switch( modelinfo->GetModelType( model ) )
			{
				case mod_studio:
					Grid_RunStudioShaderReplacement( model );
					break;

				case mod_brush:
					Grid_RunBrushShaderReplacement( model, false );
					break;

				default:
					break;
			}
			
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void Grid_PerformReplacement( IMaterial *material )
{
	if( !material || gModifiedMaterials.Find( AllocPooledString( material->GetName() ) ) != gModifiedMaterials.InvalidIndex() )
	{
		return;
	}

	// Check if the current material matches an entry in the shader table.
	const char *shaderName = material->GetShaderName();
	FOR_EACH_MAP( gShaderReplacementTable, i )
	{
		const char *findShaderName = STRING( gShaderReplacementTable.Key( i ) );
		if( !Q_stricmp( shaderName, findShaderName ) )
		{
			ConColorMsg( COLOR_BLUE, "Patching material %s (%s -> %s)\n", material->GetName(), findShaderName, STRING( gShaderReplacementTable[i] ) );

			// Setting the shader by itself reverts all material params to their default values.
			// So we must make a copy of the current settings before applying the new shader.
			KeyValues *vars = Grid_MaterialParamsToKeyValues( STRING( gShaderReplacementTable[i] ), material->GetShaderParams(), material->ShaderParamCount() );
			int flags = Grid_MaterialFlagsToInt( material );

			material->SetShaderAndParams( vars );
			Grid_IntToMaterialFlags( flags, material );

			vars->deleteThis();

			gModifiedMaterials.Insert( AllocPooledString( material->GetName() ), true );
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Shamelessly ripped (and tweaked) from `ToolFramework_RecordMaterialParams`.
//-----------------------------------------------------------------------------
static KeyValues *Grid_MaterialParamsToKeyValues( const char *shader, IMaterialVar **vars, int count )
{
	KeyValues *msg = new KeyValues( shader );
	char str[ 256 ];

	for ( int i = 0; i < count; ++i )
	{
		IMaterialVar *pVar = vars[ i ];
		const char *pVarName = pVar->GetName();

		if( !Q_stricmp( pVarName, "$flags" ) || !Q_stricmp( pVarName, "$flags_defined" ) ||
			!Q_stricmp( pVarName, "$flags2" ) || !Q_stricmp( pVarName, "$flags_defined2" ))
		{
			continue;
		}

		MaterialVarType_t vartype = pVar->GetType();
		switch ( vartype )
		{
		case MATERIAL_VAR_TYPE_FLOAT:
			msg->SetFloat( pVarName, pVar->GetFloatValue() );
			break;

		case MATERIAL_VAR_TYPE_INT:
			msg->SetInt( pVarName, pVar->GetIntValue() );
			break;

		case MATERIAL_VAR_TYPE_STRING:
			msg->SetString( pVarName, pVar->GetStringValue() );
			break;

		case MATERIAL_VAR_TYPE_FOURCC:
			Assert( 0 ); // JDTODO
			break;

		case MATERIAL_VAR_TYPE_VECTOR:
			{
				const float *pVal = pVar->GetVecValue();
				int dim = pVar->VectorSize();
				switch ( dim )
				{
				case 2:
					V_snprintf( str, sizeof( str ), "[%f %f]", pVal[ 0 ], pVal[ 1 ] );
					break;
				case 3:
					V_snprintf( str, sizeof( str ), "[%f %f %f]", pVal[ 0 ], pVal[ 1 ], pVal[ 2 ] );
					break;
				case 4:
					V_snprintf( str, sizeof( str ), "[%f %f %f %f]", pVal[ 0 ], pVal[ 1 ], pVal[ 2 ], pVal[ 3 ] );
					break;
				default:
					Assert( 0 );
					*str = 0;
				}
				msg->SetString( pVarName, str );
			}
			break;

		case MATERIAL_VAR_TYPE_MATRIX:
			{
				const VMatrix &matrix = pVar->GetMatrixValue();
				const float *pVal = matrix.Base();
				V_snprintf( str, sizeof( str ),
					"[%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f]",
					pVal[ 0 ],  pVal[ 1 ],  pVal[ 2 ],  pVal[ 3 ],
					pVal[ 4 ],  pVal[ 5 ],  pVal[ 6 ],  pVal[ 7 ],
					pVal[ 8 ],  pVal[ 9 ],  pVal[ 10 ], pVal[ 11 ],
					pVal[ 12 ], pVal[ 13 ], pVal[ 14 ], pVal[ 15 ] );
				msg->SetString( pVarName, str );
			}
			break;

		case MATERIAL_VAR_TYPE_TEXTURE:
			msg->SetString( pVarName, pVar->GetTextureValue()->GetName() );
			break;

		case MATERIAL_VAR_TYPE_MATERIAL:
			msg->SetString( pVarName, pVar->GetTextureValue()->GetName() );
			break;

		case MATERIAL_VAR_TYPE_UNDEFINED:
			//			Assert( 0 ); // these appear to be (mostly? all?) textures, although I don't know why they're not caught by the texture case above...
			break; // JDTODO

		default:
			Assert( 0 );
		}
	}

	return msg;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static int Grid_MaterialFlagsToInt( IMaterial *material )
{
	int flags = 0;

	for( int i = 0; i < 32; i++ )
	{
		MaterialVarFlags_t f = (MaterialVarFlags_t)( 1 << i );
		bool set = material->GetMaterialVarFlag( f );

		if( set )
		{
			flags |= f;
		}
	}

	return flags;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void Grid_IntToMaterialFlags( int flags, IMaterial *material )
{
	for( int i = 0; i < 32; i++ )
	{
		MaterialVarFlags_t f = (MaterialVarFlags_t)( 1 << i );
		bool set = flags & (1 << i) ? true : false;

		material->SetMaterialVarFlag( f, set );
	}
}