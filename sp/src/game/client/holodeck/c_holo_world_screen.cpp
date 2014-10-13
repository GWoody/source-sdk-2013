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
#include "in_leap.h"

#include "engine/ivdebugoverlay.h"
#include "cdll_client_int.h"
#include "iclientmode.h"
#include "vgui/IInputInternal.h"
#include "in_buttons.h"

#include <vgui_controls/Button.h>
#include <vgui_controls/Slider.h>

using namespace vgui;

static ConVar holo_screen_distance( "holo_screen_distance", "20", FCVAR_ARCHIVE );
static ConVar holo_screen_height_offset( "holo_screen_height_offset", "56", FCVAR_ARCHIVE );
static ConVar holo_screen_finger_tipdir_tolertance( "holo_screen_finger_tipdir_tolertance", "30", FCVAR_ARCHIVE );
static ConVar holo_screen_button_velocity( "holo_screen_button_velocity", "5", FCVAR_ARCHIVE );
static ConVar holo_screen_touch_distance( "holo_screen_touch_distance", "2", FCVAR_ARCHIVE );
static ConVar holo_screen_slider_pinch( "holo_screen_slider_pinch", "0.75", FCVAR_ARCHIVE );

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
	_interacted = 0;
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

	CFrame frame = CLeapMotion::Get().GetLastFrame();
	frame.ToEntitySpace( C_HoloPlayer::GetLocalPlayer(), Vector( 0, 0, 60 ) );

	for( int i = 0; i < HAND_COUNT; i++ )
	{
		const CHand &hand = frame.GetHand( (EHand)i );
		CheckHandContact( pPanel, hand );
	}
}

//-----------------------------------------------------------------------------
// We only need to support the pointer. Only a fool would use these screens
// with any other finger.
//-----------------------------------------------------------------------------
void C_HoloWorldScreen::CheckHandContact( vgui::Panel *panel, const CHand &hand )
{
	//for( int i = 0; i < FINGER_COUNT; i++ )
	{
		int i = FINGER_POINTER;
		const CFinger &finger = hand.GetFingerByType( (EFinger)i );

		if( CheckFingerContact( panel, hand, finger ) )
		{
			//break;
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool C_HoloWorldScreen::CheckFingerContact( vgui::Panel *panel, const CHand &hand, const CFinger &finger )
{
	C_HoloPlayer *player = C_HoloPlayer::GetLocalPlayer();

	// Generate a ray along the view direction
	const Vector &fingertipPos = finger.GetTipPosition();
	const Vector &fingerDir = ( fingertipPos - player->EyePosition() ).Normalized();

	// Compute cursor position...
	Ray_t lookDir;
	Vector endPos;
	
	float u, v;

	// Viewmodel attached screens that take input need to have a moving cursor
	// Do a pick under the cursor as our selection
	VectorMA( fingertipPos, 3.0f, fingerDir, endPos );
	lookDir.Init( fingertipPos, endPos );
	
	if (!IntersectWithRay( lookDir, &u, &v, NULL ))
		return false;

	if ( ((u < 0) || (v < 0) || (u > 1) || (v > 1)) && !m_bLoseThinkNextFrame)
		return false;

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

	Vector intersection = GetPanelIntersectionPosition( u, v );
	Vector delta = intersection - fingertipPos;
	CheckChildCollision( panel, hand, finger, px, py, delta.Length() );

	g_pClientMode->DeactivateInGameVGuiContext();
	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Vector C_HoloWorldScreen::GetPanelIntersectionPosition( float u, float v )
{
	Vector upl, upr, lwl;
	ComputeEdges( &upl, &upr, &lwl );

	Vector xdir = upr - upl;
	Vector ydir = lwl - upl;

	Vector pt = upl;
	pt += u * xdir;
	pt += v * ydir;

	return pt;
}

//-----------------------------------------------------------------------------
// This method is a massive hack because its a workaround for Valves broken 
// VGUI input code.
// Basically there's a bug that makes input not work on world space VGUI panels
// (the bug exists within the private engine section, and cannot be properly fixed
// without engine access).
//-----------------------------------------------------------------------------
void C_HoloWorldScreen::CheckChildCollision( Panel *panel, const CHand &hand, const CFinger &finger, int px, int py, float distance )
{
	bool closeEnough = distance < holo_screen_touch_distance.GetFloat();
	bool fastEnough = distance < holo_screen_touch_distance.GetFloat() * 4 && finger.GetVelocityDirectionTheta() < holo_screen_finger_tipdir_tolertance.GetFloat() && finger.GetTipVelocity().Length() > 10;

	if( !closeEnough && !fastEnough )
	{
		Slider *slider = dynamic_cast<Slider *>( ipanel()->GetPanel( _interacted, panel->GetModuleName() ) );
		if( !slider )
		{
			// Sliders only release when we release pinch.
			_interacted = 0;
		}
	}

	for (int i = 0; i < panel->GetChildCount(); i++)
	{
		Panel *child = panel->GetChild( i );

		if( _interacted && _interacted != child->GetVPanel() )
		{
			// This is not the panel we are currently interacting with.
			continue;
		}

		CheckButton( child, px, py, closeEnough, fastEnough );
		CheckSlider( hand, finger, child, px, py, distance );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_HoloWorldScreen::CheckButton( vgui::Panel *child, int px, int py, bool closeEnough, bool fastEnough )
{
	Button *button = dynamic_cast<Button*>( child );
	if( !button )
	{
		return;
	}

	button->SetArmed( false );

	// Generate mouse input commands
	if( CheckBoundingRect( px, py, button ) )
	{
		// Finger is close enough to be touching.
		if( closeEnough || fastEnough )
		{
			if( !button->IsDepressed() )
			{
				// Only simulate a press on the first contact.
				button->DoClick();
			}

			// Keep the button pressed on prolonged contact.
			button->ForceDepressed( true );
			_interacted = button->GetVPanel();
		}
		else
		{
			// Finger is over the control, but not touching. Highlight it.
			button->SetArmed( true );
		}
	}
	else
	{
		button->ForceDepressed( false );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_HoloWorldScreen::CheckSlider( const CHand &hand, const CFinger &finger, vgui::Panel *child, int px, int py, float distance )
{
	Slider *slider = dynamic_cast<Slider*>( child );
	if( !slider )
	{
		return;
	}

	// Assume an invalid interaction.
	VPANEL oldInteracted = _interacted;
	_interacted = 0;

	if( hand.GetPinchStrength() > holo_screen_slider_pinch.GetFloat() && distance < holo_screen_touch_distance.GetFloat() * 4 && hand.GetClosestFingerTo( FINGER_THUMB ).GetType() == FINGER_POINTER )
	{
		int x1, x2, y1, y2;
		slider->GetBounds( x1, y1, x2, y2 );

		if( oldInteracted || CheckBoundingRect( px, py, slider ) )
		{
			float perc = (float)( px - x1 ) / (float)x2;

			int min, max;
			slider->GetRange( min, max );

			slider->SetValue( min + (perc * max) );
			_interacted = slider->GetVPanel();
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool C_HoloWorldScreen::CheckBoundingRect( int px, int py, Panel *panel )
{
	int x1, x2, y1, y2;
	panel->GetBounds( x1, y1, x2, y2 );

	return ( px >= x1 && px <= x1 + x2 && py >= y1 && py <= y1 + y2 );
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