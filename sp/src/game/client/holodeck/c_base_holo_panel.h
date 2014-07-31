/*
===============================================================================

	c_base_holo_panel.h
		Implements client side rendering common to all holo panels.

===============================================================================
*/

#ifndef __C_BASE_HOLO_PANEL_H__
#define __C_BASE_HOLO_PANEL_H__

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_BaseHoloPanel : public C_BaseEntity
{
public:
	DECLARE_CLASS( C_BaseHoloPanel, C_BaseEntity );
	DECLARE_CLIENTCLASS();

	// C_BaseEntity overrides.
	virtual void	Simulate();

private:
	Vector4D		CalculateAnimatedSpriteColor() const;

	EHANDLE			_animation;
};

#endif // __C_BASE_HOLO_PANEL_H__