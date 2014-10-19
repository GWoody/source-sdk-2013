/*
===============================================================================

	c_grid_rendertargets.h
	Creates the VGUI 256x256 RT.

===============================================================================
*/

#ifndef __C_GRID_RENDERTARGETS_H__
#define __C_GRID_RENDERTARGETS_H__

#include "baseclientrendertargets.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridRenderTargets : public CBaseClientRenderTargets
{
	DECLARE_CLASS_GAMEROOT( CGridRenderTargets, CBaseClientRenderTargets );

public:
	virtual void	InitClientRenderTargets( IMaterialSystem *pMaterialSystem, IMaterialSystemHardwareConfig *pHardwareConfig );
	virtual void	ShutdownClientRenderTargets();

	ITexture *		GetVGuiTexture();

private:
	ITexture *		CreateVGuiTexture( IMaterialSystem *pMaterialSystem );

	CTextureReference	_VGuiTexture;
};

extern CGridRenderTargets *g_pGridRenderTargets;

#endif