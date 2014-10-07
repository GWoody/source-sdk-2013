/*
===============================================================================

	c_holo_world_screen.h
	Implements the base class for a panel in world space that is a child of the player.

===============================================================================
*/

#ifndef __C_HOLO_WORLD_SCREEN_H__
#define __C_HOLO_WORLD_SCREEN_H__

#include "c_vguiscreen.h"

class C_HoloHand;
class vgui::Panel;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_HoloWorldScreen : public C_VGuiScreen
{
public:
	DECLARE_CLASS( C_HoloWorldScreen, C_VGuiScreen );
	DECLARE_CLIENTCLASS();

	C_HoloWorldScreen();
	
	// C_VGuiScreen overrides.
	virtual void	ClientThink();
	virtual void	GetAimEntOrigin( IClientEntity *pAttachedTo, Vector *pOrigin, QAngle *pAngles );

private:
	void			CheckHandContact( vgui::Panel *panel, C_HoloHand *hand );
	void			CheckChildCollision( vgui::Panel *panel, int px, int py );

	float			CalculateLeftOffset() const;

	int				_type;
};

#endif // __C_HOLO_WORLD_SCREEN_H__