/*
===============================================================================

	c_holo_world_screen.cpp
	Implements the base class for a panel in world space that is a child of the player.

===============================================================================
*/

#include "cbase.h"
#include "holodeck/holo_shared.h"
#include "c_holo_world_screen.h"
#include "c_holo_player.h"
#include "c_holo_hand.h"

#include "engine/ivdebugoverlay.h"
#include "cdll_client_int.h"
#include "iclientmode.h"
#include "vgui/IInputInternal.h"
#include "in_buttons.h"

#include <vgui_controls/Button.h>

using namespace vgui;

static ConVar holo_screen_distance( "holo_screen_distance", "32", FCVAR_ARCHIVE );
static ConVar holo_screen_height_offset( "holo_screen_height_offset", "48", FCVAR_ARCHIVE );

extern vgui::IInputInternal *g_InputInternal;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IMPLEMENT_CLIENTCLASS_DT( C_HoloWorldScreen, DT_HoloWorldScreen, CHoloWorldScreen )

	RecvPropInt( RECVINFO( _type ) ),

END_RECV_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
C_HoloWorldScreen::C_HoloWorldScreen()
{
}

//-----------------------------------------------------------------------------
// Purpose: Deal with input
//-----------------------------------------------------------------------------
void C_HoloWorldScreen::ClientThink( void )
{
	int nButtonsChanged = m_nOldButtonState ^ m_nButtonState;

	m_nOldButtonState = m_nButtonState;

	// Debounced button codes for pressed/released
	m_nButtonPressed =  nButtonsChanged & m_nButtonState;		// The changed ones still down are "pressed"
	m_nButtonReleased = nButtonsChanged & (~m_nButtonState);	// The ones not down are "released"

	// Bypass C_VGuiScreen.
	C_BaseEntity::ClientThink();

	vgui::Panel *pPanel = m_PanelWrapper.GetPanel();
	if (!pPanel)
		return;
	
	C_HoloPlayer *pLocalPlayer = C_HoloPlayer::GetLocalPlayer();
	if (!pLocalPlayer)
		return;

	for( int i = 0; i < HAND_COUNT; i++ )
	{
		C_HoloHand *hand = pLocalPlayer->GetHand( (EHand)i );
		CheckHandContact( pPanel, hand );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_HoloWorldScreen::CheckHandContact( vgui::Panel *panel, C_HoloHand *hand )
{
	C_HoloPlayer *pLocalPlayer = C_HoloPlayer::GetLocalPlayer();

	// Generate a ray along the view direction
	Vector vecEyePosition = pLocalPlayer->EyePosition();
	
	QAngle viewAngles = pLocalPlayer->EyeAngles();

	// Compute cursor position...
	Ray_t lookDir;
	Vector endPos;
	
	float u, v;

	// Viewmodel attached screens that take input need to have a moving cursor
	// Do a pick under the cursor as our selection
	Vector viewDir;
	AngleVectors( viewAngles, &viewDir );
	VectorMA( vecEyePosition, 1000.0f, viewDir, endPos );
	lookDir.Init( vecEyePosition, endPos );

	if (!IntersectWithRay( lookDir, &u, &v, NULL ))
		return;

	if ( ((u < 0) || (v < 0) || (u > 1) || (v > 1)) && !m_bLoseThinkNextFrame)
		return;

	// This will cause our panel to grab all input!
	g_pClientMode->ActivateInGameVGuiContext( panel );

	// Convert (u,v) into (px,py)
	int px = (int)(u * m_nPixelWidth + 0.5f);
	int py = (int)(v * m_nPixelHeight + 0.5f);

	// Generate mouse input commands
	if ((px != m_nOldPx) || (py != m_nOldPy))
	{
		g_InputInternal->InternalCursorMoved( px, py );
		m_nOldPx = px;
		m_nOldPy = py;
	}

	CheckChildCollision( panel, px, py );

	g_pClientMode->DeactivateInGameVGuiContext();
}

//-----------------------------------------------------------------------------
// This method is a massive hack because its a workaround for Valves broken 
// VGUI input code.
// Basically there's a bug that makes input not work on world space VGUI panels
// (the bug exists within the private engine section, and cannot be properly fixed
// without engine access).
//-----------------------------------------------------------------------------
void C_HoloWorldScreen::CheckChildCollision( vgui::Panel *panel, int px, int py )
{
	for (int i = 0; i < panel->GetChildCount(); i++)
	{
		vgui::Button *child = dynamic_cast<vgui::Button*>( panel->GetChild(i) );
		if ( child )
		{
			int x1, x2, y1, y2;
			child->GetBounds( x1, y1, x2, y2 );

			// Generate mouse input commands
			if ( px >= x1 && px <= x1 + x2 && py >= y1 && py <= y1 + y2 )
			{
				if ( m_nButtonPressed & IN_ATTACK )
				{
					child->DoClick();
				}
				if ( m_nButtonState & IN_ATTACK )
				{
					child->ForceDepressed( true );
				}
				if ( m_nButtonReleased & IN_ATTACK )
				{
					child->ForceDepressed( false );
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_HoloWorldScreen::GetAimEntOrigin( IClientEntity *pAttachedTo, Vector *pOrigin, QAngle *pAngles )
{
	BaseClass::GetAimEntOrigin( pAttachedTo, pOrigin, pAngles );
	Vector originalOrigin = *pOrigin;
	float originalYaw = (*pAngles)[YAW];

	float extraLeftOffset = CalculateLeftOffset();

	//
	// Calculate angle.
	//
	{
		// Build the offset from the panel to the player (taking rotation into account).
		Vector offset( holo_screen_distance.GetFloat(), extraLeftOffset, 0.0f );
		VectorYawRotate( offset, originalYaw, offset );

		// Get direction(player, screen) as a 2D vector.
		Vector dir = pAttachedTo->GetAbsOrigin() - ( originalOrigin + offset );
		dir.AsVector2D().NormalizeInPlace();
		QAngle angles;
		VectorAngles( dir, angles );

		// Add a little fudge value to make the screen face the player.
		(*pAngles) = angles + QAngle( 0, 90, 90 );
	}

	//
	// Calculate offset.
	//
	{
		// Build the offset from the panel to the player (taking rotation into account).
		Vector offset( holo_screen_distance.GetFloat(), (m_flWidth / 2.0f) + extraLeftOffset, 0.0f );

		if( _type == WORLD_PANEL_RIGHT )
		{
			float screenYaw = RAD2DEG( atan2( holo_screen_distance.GetFloat(), m_flWidth ) );

			QAngle panelAngle = QAngle( 0.0f, -screenYaw, 0.0f );
			Vector panelDir;
			AngleVectors( panelAngle, &panelDir );
			panelDir.z = 0;
			panelDir.AsVector2D().NormalizeInPlace();

			offset = Vector( holo_screen_distance.GetFloat(), m_flWidth / 2.0f, 0.0f );
			offset -= panelDir * m_flWidth;
		}

		VectorYawRotate( offset, originalYaw, offset );

		(*pOrigin) = originalOrigin + offset;
		pOrigin->z += holo_screen_height_offset.GetInt();
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float C_HoloWorldScreen::CalculateLeftOffset() const
{
	switch( _type )
	{
		case WORLD_PANEL_LEFT:
			return -m_flWidth;

		case WORLD_PANEL_RIGHT:
			return m_flWidth;

		case WORLD_PANEL_MIDDLE:
		default:
			break;
	}

	return 0;
}