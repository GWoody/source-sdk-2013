/*
===============================================================================

	c_grid_rendertargets.cpp
	Creates the VGUI 256x256 RT.

===============================================================================
*/

#include "cbase.h"
#include "c_grid_rendertargets.h"
#include "materialsystem/itexture.h"

static CGridRenderTargets gRenderTargets;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CGridRenderTargets, IClientRenderTargets, CLIENTRENDERTARGETS_INTERFACE_VERSION, gRenderTargets );
CGridRenderTargets* g_pGridRenderTargets = &gRenderTargets;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridRenderTargets::InitClientRenderTargets( IMaterialSystem *pMaterialSystem, IMaterialSystemHardwareConfig *pHardwareConfig )
{
	_VGuiTexture.Init( CreateVGuiTexture( pMaterialSystem ) );

	BaseClass::InitClientRenderTargets( pMaterialSystem, pHardwareConfig );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridRenderTargets::ShutdownClientRenderTargets()
{
	_VGuiTexture.Shutdown();

	BaseClass::ShutdownClientRenderTargets();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ITexture *CGridRenderTargets::GetVGuiTexture()
{
	if( !_VGuiTexture )
	{
		_VGuiTexture.InitRenderTarget( 256, 256, RT_SIZE_OFFSCREEN, IMAGE_FORMAT_RGBA8888, MATERIAL_RT_DEPTH_SHARED, false );
		Assert( !IsErrorTexture( _VGuiTexture ) );
	}

	return _VGuiTexture;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ITexture *CGridRenderTargets::CreateVGuiTexture( IMaterialSystem *pMaterialSystem )
{
	return pMaterialSystem->CreateNamedRenderTargetTextureEx( "_rt_VGuiModelPanel", 256, 256, RT_SIZE_OFFSCREEN, IMAGE_FORMAT_RGBA8888, MATERIAL_RT_DEPTH_SHARED, TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT, 0 );
}