/*
===============================================================================

	c_grid_estate_prop_parser.cpp
	Creates screenshots of all props in the estate demo.

===============================================================================
*/

#include "cbase.h"
#include "filesystem.h"
#include "tier1/fmtstr.h"
#include "view_shared.h"
#include "view.h"
#include "materialsystem/itexture.h"
#include "grid/c_grid_rendertargets.h"
#include "model_types.h"
#include "basemodelpanel.h"

#include "bitmap/tgaloader.h"
#include "bitmap/bitmap.h"
#include "bitmap/tgawriter.h"
#include "ivtex.h"

#include <stdio.h>

IVTex *VTex_Load( CSysModule** pModule )
{
	// load the vtex dll
	IVTex *pIVTex = NULL;
	*pModule = g_pFullFileSystem->LoadModule( "vtex_dll" );
	if ( *pModule )
	{
		CreateInterfaceFn factory = Sys_GetFactory( *pModule );
		if ( factory )
		{
			pIVTex = ( IVTex * )factory( IVTEX_VERSION_STRING, NULL );
		}
	}

	if ( !pIVTex )
	{
		ConMsg( "Can't load vtex_dll.dll\n" );
	}

	return pIVTex;
}

void VTex_Unload( CSysModule *pModule )
{
	g_pFullFileSystem->UnloadModule( pModule );
}

void *VTex_FS_Factory( const char *name, int *code )
{
	if( !Q_stricmp( name, FILESYSTEM_INTERFACE_VERSION ) )
	{
		return g_pFullFileSystem;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridEstatePropParser : CAutoGameSystem
{
public:
	C_GridEstatePropParser() : CAutoGameSystem( "C_GridEstatePropParser" )
	{
	}

	virtual void LevelInitPostEntity()
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
				ParsePropFile( sub->GetString() );
			}
		}

		kv->deleteThis();
	}

private:
	void ParsePropFile( const char *script )
	{
		KeyValues *kv = new KeyValues( "props" );
		if( !kv->LoadFromFile( g_pFullFileSystem, script, "MOD" ) )
		{
			Assert( 0 );
			ConColorMsg( COLOR_YELLOW, __FUNCTION__": failed to open \"%s\"\n", script );
			return;
		}

		CSysModule *module;
		IVTex *vtex = VTex_Load( &module );

		FOR_EACH_TRUE_SUBKEY( kv, sub )
		{
			const char *modelname = sub->GetName();
			ScreenshotModel( vtex, modelname );
		}

		VTex_Unload( module );

		kv->deleteThis();
	}

	void ScreenshotModel( IVTex *vtex, const char *model )
	{
		//
		// Create paths.
		//
		char modelNoExt[MAX_PATH];
		char modelNoFile[MAX_PATH];

		Q_strncpy( modelNoFile, model, sizeof(modelNoFile) );
		Q_StripFilename( modelNoFile );
		Q_StripExtension( model, modelNoExt, sizeof(modelNoExt) );

		CFmtStr vtfPath( "materials/grid/%s.vtf", modelNoExt );
		CFmtStr vmtPath( "materials/grid/%s.vmt", modelNoExt );
		if( filesystem->FileExists( vtfPath, "MOD" ) )
		{
			PrecacheMaterial( vmtPath.Get() );
			return;
		}

		//
		// Create the model.
		//
		CModelPanelModel *pEnt = new CModelPanelModel;
		if ( pEnt->InitializeAsClientEntity( model, RENDER_GROUP_OPAQUE_ENTITY ) == false )
		{
			ConColorMsg( COLOR_YELLOW, __FUNCTION__": InitializeAsClientEntity failed!\n" );
			pEnt->Remove();
			return;
		}

		QAngle angles( 0.0f, 200.0f, 0.0f );
		Vector origin( 64.0f, 40.0f, -15.0f );

		pEnt->AddEffects( EF_NODRAW );
		pEnt->SetAbsOrigin( origin );
		pEnt->SetLocalAngles( angles );

		//
		// Set the view.
		//
		CViewSetup view;
		view.x = view.y = 0;
		view.width = view.height = 256;
		view.m_bOrtho = false;
		view.fov = ScaleFOVByWidthRatio( 60.0f, 1.0f );
		view.origin = vec3_origin;
		view.angles.Init();
		view.zNear = VIEW_NEARZ;
		view.zFar = 1000;

		//
		// Set the renderer.
		//
		CMatRenderContextPtr pRenderContext( materials );
		pRenderContext->SetLightingOrigin( vec3_origin );
		pRenderContext->SetAmbientLight( 0.4, 0.4, 0.4 );

		static Vector white[6] = 
		{
			Vector( 0.4, 0.4, 0.4 ),
			Vector( 0.4, 0.4, 0.4 ),
			Vector( 0.4, 0.4, 0.4 ),
			Vector( 0.4, 0.4, 0.4 ),
			Vector( 0.4, 0.4, 0.4 ),
			Vector( 0.4, 0.4, 0.4 ),
		};

		g_pStudioRender->SetAmbientLightColors( white );
		g_pStudioRender->SetLocalLights( 0, NULL );

		//
		// Draw the model.
		//
		pRenderContext->PushRenderTargetAndViewport( g_pGridRenderTargets->GetVGuiTexture(), 0, 0, 256, 256 );

		pRenderContext->ClearColor4ub( 0, 0, 0, 0 );
		pRenderContext->ClearBuffers( true, true );

		Frustum dummyFrustum;
		render->Push3DView( view, VIEW_CLEAR_COLOR | VIEW_CLEAR_DEPTH, g_pGridRenderTargets->GetVGuiTexture(), dummyFrustum );

		modelrender->SuppressEngineLighting( true );
		float color[3] = { 1.0f, 1.0f, 1.0f };
		render->SetColorModulation( color );
		render->SetBlend( 1.0f );
		pEnt->DrawModel( STUDIO_RENDER );

		//
		// Cleanup.
		//
		modelrender->SuppressEngineLighting( false );
		render->PopView( dummyFrustum );
		pEnt->Remove();
		pRenderContext->PopRenderTargetAndViewport();

		//
		// Write the base TGA. This also contains the depth image.
		//
		CFmtStr tgaPath( "materialsrc/grid/%s.tga", modelNoExt );
		ConColorMsg( COLOR_BLUE, "Writing %s\n", tgaPath.Get() );
		filesystem->CreateDirHierarchy( CFmtStr( "materialsrc/grid/%s", modelNoFile ), "MOD" );
		g_pGridRenderTargets->GetVGuiTexture()->SaveToFile( tgaPath.Get() );

		//
		// Strip the depth from the TGA.
		//
		int width, height;
		CUtlMemory<unsigned char> pixels;
		Assert( TGALoader::LoadRGBA8888( tgaPath, pixels, width, height ) );

		// Fix alpha.
		for( int i = 0; i < pixels.Count(); i += 4 )
		{
			if( pixels[i] || pixels[i+1] || pixels[i+2] )
			{
				pixels[i+3] = 255;
			}
			else
			{
				pixels[i+3] = 196;
			}
		}

		int stride = width * 4;
		unsigned char *colorbuffer = pixels.Base() + stride * (height / 2);
		Assert( TGAWriter::WriteTGAFile( tgaPath, width, height / 2, IMAGE_FORMAT_RGBA8888, colorbuffer, stride ) );

		//
		// Generate VTex script.
		//
		CFmtStr txtPath( "materialsrc/grid/%s.txt", modelNoExt );
		FileHandle_t fp = g_pFullFileSystem->Open( txtPath, "w" );
		g_pFullFileSystem->FPrintf( fp, "nonice 1\nnolod 1\nnomip 1\n" );
		g_pFullFileSystem->Close( fp );

		//
		// Compile VTF.
		//
		char fullpath[MAX_PATH];
		g_pFullFileSystem->RelativePathToFullPath( txtPath, "MOD", fullpath, sizeof(fullpath) );

		char *argv[64];
		int iArg = 0;
		argv[iArg++] = "";
		argv[iArg++] = "-quiet";
		argv[iArg++] = "-UseStandardError";
		argv[iArg++] = fullpath;

		vtex->VTex( VTex_FS_Factory, engine->GetGameDirectory(), iArg, argv );

		//
		// Generate VMT.
		//
		CFmtStr vmtSource( 
"UnlitGeneric\n\
{\n\
	\"$basetexture\"	\"grid/%s.vtf\"\n\
	\"$translucent\"	1\n\
}\n", modelNoExt );

		fp = g_pFullFileSystem->Open( vmtPath, "w" );
		g_pFullFileSystem->FPrintf( fp, vmtSource.Get() );
		g_pFullFileSystem->Close( fp );

		PrecacheMaterial( vmtPath.Get() );
	}
};

C_GridEstatePropParser gGridEstatePropParser;