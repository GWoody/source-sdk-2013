/*
===============================================================================

	base_grabcontroller.h
		Mostly borrowed from `hl2/weapon_physcannon.cpp`

===============================================================================
*/

#ifndef __GRID_PICKUPCONTROLLER_H__
#define __GRID_PICKUPCONTROLLER_H__

#include "base_grabcontroller.h"

class CBaseHoloHand;

//-----------------------------------------------------------------------------
// Player pickup controller
//-----------------------------------------------------------------------------
class CPlayerPickupController : public CBaseEntity
{
	DECLARE_DATADESC();
	DECLARE_CLASS( CPlayerPickupController, CBaseEntity );
public:
	void Init( CBaseHoloHand *pPlayer, CBaseEntity *pObject );
	void Shutdown( bool bThrown = false );
	bool OnControls( CBaseEntity *pControls ) { return true; }
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void OnRestore()
	{
		m_grabController.OnRestore();
	}
	void VPhysicsUpdate( IPhysicsObject *pPhysics ){}
	void VPhysicsShadowUpdate( IPhysicsObject *pPhysics ) {}

	bool IsHoldingEntity( CBaseEntity *pEnt );
	CGrabController &GetGrabController() { return m_grabController; }

private:
	CGrabController		m_grabController;
	CBaseHoloHand		*m_pHand;
};


#endif // __GRID_PICKUPCONTROLLER_H__