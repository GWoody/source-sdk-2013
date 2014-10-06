/*
===============================================================================

	c_holo_world_screen.cpp
	Implements the base class for a panel in world space that is a child of the player.

===============================================================================
*/

#include "cbase.h"
#include "holodeck/holo_shared.h"
#include "c_holo_world_screen.h"
#include "engine/ivdebugoverlay.h"
#include "cdll_client_int.h"

using namespace vgui;
using namespace holo;

static ConVar holo_screen_distance( "holo_screen_distance", "32", FCVAR_ARCHIVE );
static ConVar holo_screen_height_offset( "holo_screen_height_offset", "48", FCVAR_ARCHIVE );

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