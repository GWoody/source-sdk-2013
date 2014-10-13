/*
===============================================================================

	c_holo_world_screen.h
	Implements the base class for a panel in world space that is a child of the player.

===============================================================================
*/

#ifndef __C_HOLO_WORLD_SCREEN_H__
#define __C_HOLO_WORLD_SCREEN_H__

#include "c_vguiscreen.h"
#include "vgui/VGUI.h"

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
	virtual RenderGroup_t	GetRenderGroup()		{ return RENDER_GROUP_TRANSLUCENT_ENTITY; }
	virtual void	ClientThink();
	virtual void	GetAimEntOrigin( IClientEntity *pAttachedTo, Vector *pOrigin, QAngle *pAngles );
	virtual EWorldPanel	GetWorldPanelType() const	{ return (EWorldPanel)_type; }

private:
	void			CheckHandContact( vgui::Panel *panel, const CHand &hand );
	bool			CheckFingerContact( vgui::Panel *panel, const CHand &hand, const CFinger &finger );
	void			CheckChildCollision( vgui::Panel *panel, const CHand &hand, const CFinger &finger, int px, int py, float distance );
	Vector			GetPanelIntersectionPosition( float u, float v );

	void			CheckButton( vgui::Panel *child, int px, int py, bool closeEnough, bool fastEnough );
	void			CheckSlider( const CHand &hand, const CFinger &finger, vgui::Panel *child, int px, int py );

	float			CalculateLeftOffset() const;

	int				_type;

	vgui::VPANEL	_interacted;
};

#endif // __C_HOLO_WORLD_SCREEN_H__