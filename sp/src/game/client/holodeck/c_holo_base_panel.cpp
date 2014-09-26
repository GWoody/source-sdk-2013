/*
===============================================================================

	c_base_holo_panel.cpp
		Implements client side rendering common to all holo panels.

===============================================================================
*/

#include "cbase.h"
#include "c_holo_base_panel.h"

//-----------------------------------------------------------------------------
// Specify what this class is receiving from the server.
//-----------------------------------------------------------------------------
IMPLEMENT_CLIENTCLASS_DT( C_BaseHoloPanel, DT_BaseHoloPanel, CBaseHoloPanel )

	RecvPropEHandle( RECVINFO( _animation ) ),

END_RECV_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_BaseHoloPanel::Simulate()
{
	if( _animation )
	{
		Vector4D color = CalculateAnimatedSpriteColor();
		_animation->SetRenderColor( color.x * 255.0f, color.y * 255.0f, color.z * 255.0f, color.w * 255.0f );
	}

	BaseClass::Simulate();
}

//-----------------------------------------------------------------------------
// Determines the color of the animated sprite for this frame.
//-----------------------------------------------------------------------------
Vector4D C_BaseHoloPanel::CalculateAnimatedSpriteColor() const
{
	C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();
	if( !player )
	{
		return Vector4D( 0.0f, 0.0f, 0.0f, 0.0f );
	}

	Vector direction = player->GetAbsOrigin() - GetAbsOrigin();
	float distance = direction.Length();

	const float MIN_PICKUP_DISTANCE = 48.0f;
	const float MAX_PICKUP_DISTANCE = 192.0f;
	const float MAX_VISIBLE_DISTANCE = 400.0f;

	// The object is close enough to be picked up by the player.
	// Make it bright green.
	if( distance <= MIN_PICKUP_DISTANCE )
	{
		return Vector4D( 0.0f, 1.0f, 0.0f, 1.0f );
	}

	// The player is close enough to the player for them to think they can pick it up.
	// Fade from red to green to let the player know when they are within range.
	if( distance <= MAX_PICKUP_DISTANCE )
	{
		float delta = MAX_PICKUP_DISTANCE - distance;
		float color = delta / ( MAX_PICKUP_DISTANCE - MIN_PICKUP_DISTANCE );
		return Vector4D( 1.0f - color, color, 0.0f, 1.0f );
	}

	// The object is close enough to the player for it to be highlighted.
	// Make it red, but fade transparency.
	if( distance <= MAX_VISIBLE_DISTANCE )
	{
		float delta = MAX_VISIBLE_DISTANCE - distance;
		float color = delta / ( MAX_VISIBLE_DISTANCE - MAX_PICKUP_DISTANCE );
		return Vector4D( 1.0f, 0.0f, 0.0f, color );
	}

	// The object is too far away to be of any use to the player.
	// Don't color it.
	return Vector4D( 0.0f, 0.0f, 0.0f, 0.0f );
}