/*
===============================================================================

	c_holo_player.cpp
	Client side implementation of the Holodeck player entity.

===============================================================================
*/

#include "cbase.h"
#include "c_holo_player.h"
#include "out_etactor.h"
#include "in_leap.h"

#include "c_vguiscreen.h"
#include "in_buttons.h"
#include "vgui/isurface.h"

//-----------------------------------------------------------------------------
// Network table.
//-----------------------------------------------------------------------------
IMPLEMENT_CLIENTCLASS_DT( C_HoloPlayer, DT_HoloPlayer, CHoloPlayer )

	RecvPropArray3( RECVINFO_ARRAY( _hands ), RecvPropEHandle( RECVINFO(_hands[0]) ) ),
	RecvPropVector( RECVINFO( _viewoffset ) ),

END_RECV_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
C_HoloPlayer::C_HoloPlayer()
{
	_viewoffset.Init( 0, 0, 0 );
}

//-----------------------------------------------------------------------------
// Check to see if we're in vgui input mode...
//-----------------------------------------------------------------------------
void C_HoloPlayer::DetermineVguiInputMode( CUserCmd *pCmd )
{
	// If we're dead, close down and abort!
	if ( !IsAlive() )
	{
		DeactivateVguiScreen( m_pCurrentVguiScreen.Get() );
		m_pCurrentVguiScreen.Set( NULL );
		return;
	}

	// If we're in vgui mode *and* we're holding down mouse buttons,
	// stay in vgui mode even if we're outside the screen bounds
	if (m_pCurrentVguiScreen.Get() && (pCmd->buttons & (IN_ATTACK | IN_ATTACK2)) )
	{
		SetVGuiScreenButtonState( m_pCurrentVguiScreen.Get(), pCmd->buttons );

		// Kill all attack inputs if we're in vgui screen mode
		pCmd->buttons &= ~(IN_ATTACK | IN_ATTACK2);
		return;
	}

	// We're not in vgui input mode if we're moving, or have hit a key
	// that will make us move...

	// Don't enter vgui mode if we've got combat buttons held down
	bool bAttacking = false;
	if ( ((pCmd->buttons & IN_ATTACK) || (pCmd->buttons & IN_ATTACK2)) && !m_pCurrentVguiScreen.Get() )
	{
		bAttacking = true;
	}

	// Not in vgui mode if we're pushing any movement key at all
	// Not in vgui mode if we're in a vehicle...
	// ROBIN: Disabled movement preventing VGUI screen usage
	//if ((pCmd->forwardmove > MAX_VGUI_INPUT_MODE_SPEED) ||
	//	(pCmd->sidemove > MAX_VGUI_INPUT_MODE_SPEED) ||
	//	(pCmd->upmove > MAX_VGUI_INPUT_MODE_SPEED) ||
	//	(pCmd->buttons & IN_JUMP) ||
	//	(bAttacking) )
	if ( bAttacking || IsInAVehicle() )
	{ 
		DeactivateVguiScreen( m_pCurrentVguiScreen.Get() );
		m_pCurrentVguiScreen.Set( NULL );
		return;
	}

	// Don't interact with world screens when we're in a menu
	if ( vgui::surface()->IsCursorVisible() )
	{
		DeactivateVguiScreen( m_pCurrentVguiScreen.Get() );
		m_pCurrentVguiScreen.Set( NULL );
		return;
	}

	// Not in vgui mode if there are no nearby screens
	C_BaseEntity *pOldScreen = m_pCurrentVguiScreen.Get();

	m_pCurrentVguiScreen = GetCurrentVGuiScreen( EyePosition(), pCmd->viewangles, GetTeamNumber() );

	if (pOldScreen != m_pCurrentVguiScreen)
	{
		DeactivateVguiScreen( pOldScreen );
		ActivateVguiScreen( m_pCurrentVguiScreen.Get() );
	}

	if (m_pCurrentVguiScreen.Get())
	{
		SetVGuiScreenButtonState( m_pCurrentVguiScreen.Get(), pCmd->buttons );

		// Kill all attack inputs if we're in vgui screen mode
		pCmd->buttons &= ~(IN_ATTACK | IN_ATTACK2);
	}
}

#define VGUI_SCREEN_MODE_RADIUS	80
C_BaseEntity *C_HoloPlayer::GetCurrentVGuiScreen( const Vector &viewPosition, const QAngle &viewAngle, int nTeam )
{
	if ( IsX360() )
	{
		// X360TBD: Turn this on if feature actually used
		return NULL;
	}

	C_BasePlayer *pLocalPlayer = C_BasePlayer::GetLocalPlayer();

	Assert( pLocalPlayer );

	if ( !pLocalPlayer )
		return NULL;

	CFrame frame = CLeapMotion::Get().GetLastFrame();
	frame.ToEntitySpace( this, Vector( 0, 0, 60 ) );

	const CHand &leftHand = frame.GetHand( HAND_LEFT );
	const CHand &rightHand = frame.GetHand( HAND_RIGHT );

	Ray_t pointerRays[HAND_COUNT];
	pointerRays[HAND_LEFT].Init( leftHand.GetPosition(), leftHand.GetPosition() + leftHand.GetDirection() * 2.0f * VGUI_SCREEN_MODE_RADIUS );
	pointerRays[HAND_RIGHT].Init( rightHand.GetPosition(), rightHand.GetPosition() + rightHand.GetDirection() * 2.0f * VGUI_SCREEN_MODE_RADIUS );

	Vector vecOut, vecViewDelta;

	float flBestDist = 2.0f;
	C_VGuiScreen *pBestScreen = NULL;
	extern CUtlVector<C_VGuiScreen*> g_pVGuiScreens;
	for (int i = 0; i < g_pVGuiScreens.Count(); i++ )
	{
		C_VGuiScreen *pScreen = g_pVGuiScreens[i];
		if ( pScreen->IsAttachedToViewModel() )
			continue;

		// Don't bother with screens I'm behind...
		// Hax - don't cancel backfacing with viewmodel attached screens.
		// we can get prediction bugs that make us backfacing for one frame and
		// it resets the mouse position if we lose focus.
		if ( pScreen->IsBackfacing(viewPosition) )
			continue;

		// Don't bother with screens that are turned off
		if (!pScreen->IsActive())
			continue;

		// FIXME: Should this maybe go into a derived class of some sort?
		// Don't bother with screens on the wrong team
		if (!pScreen->IsVisibleToTeam(nTeam))
			continue;

		if ( !pScreen->AcceptsInput() )
			continue;

		if ( pScreen->IsInputOnlyToOwner() && pScreen->GetPlayerOwner() != pLocalPlayer )
			continue;

		// Test perpendicular distance from the screen...
		pScreen->GetVectors( NULL, NULL, &vecOut );
		VectorSubtract( viewPosition, pScreen->GetAbsOrigin(), vecViewDelta );
		float flPerpDist = DotProduct(vecViewDelta, vecOut);
		if ( (flPerpDist < 0) || (flPerpDist > VGUI_SCREEN_MODE_RADIUS) )
			continue;

		// Perform a raycast to see where in barycentric coordinates the ray hits
		// the viewscreen; if it doesn't hit it, you're not in the mode
		for( int j = 0; j < HAND_COUNT; j++ )
		{
			float u, v, t;
			if (!pScreen->IntersectWithRay( pointerRays[j], &u, &v, &t ))
				continue;

			if ( t < flBestDist )
			{
				flBestDist = t;
				pBestScreen = pScreen;
			}
		}
	}
	
	return pBestScreen;
}