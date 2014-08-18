/*
===============================================================================

	grid_clientmode.cpp

===============================================================================
*/

#include "cbase.h"
#include "ivmodemanager.h"
#include "grid_clientmodenormal.h"
#include "panelmetaclassmgr.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// default FOV for HL2
ConVar default_fov( "default_fov", "75", FCVAR_CHEAT );

// The current client mode. Always ClientModeNormal in HL.
IClientMode *g_pClientMode = NULL;

#define SCREEN_FILE		"scripts/vgui_screens.txt"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridModeManager : public IVModeManager
{
public:
				CGridModeManager( void );
	virtual		~CGridModeManager( void );

	virtual void	Init( void );
	virtual void	SwitchMode( bool commander, bool force );
	virtual void	OverrideView( CViewSetup *pSetup );
	virtual void	CreateMove( float flInputSampleTime, CUserCmd *cmd );
	virtual void	LevelInit( const char *newmap );
	virtual void	LevelShutdown( void );
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGridModeManager::CGridModeManager( void )
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGridModeManager::~CGridModeManager( void )
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridModeManager::Init( void )
{
	g_pClientMode = GetClientModeNormal();
	PanelMetaClassMgr()->LoadMetaClassDefinitionFile( SCREEN_FILE );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridModeManager::SwitchMode( bool commander, bool force )
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridModeManager::OverrideView( CViewSetup *pSetup )
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridModeManager::CreateMove( float flInputSampleTime, CUserCmd *cmd )
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridModeManager::LevelInit( const char *newmap )
{
	g_pClientMode->LevelInit( newmap );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridModeManager::LevelShutdown( void )
{
	g_pClientMode->LevelShutdown();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static CGridModeManager g_ModeManager;
IVModeManager *modemanager = &g_ModeManager;

