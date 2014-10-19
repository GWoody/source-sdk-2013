/*
===============================================================================

	vgui_modelbutton.h
	Implements a button which has a 3D model instead of text.

===============================================================================
*/

#ifndef __VGUI_MODELBUTTON_H__
#define __VGUI_MODELBUTTON_H__

#include <vgui_controls/Button.h>
#include "game_controls/basemodelpanel.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CModelButton : public vgui::Button
{
public:
	DECLARE_CLASS_SIMPLE( CModelButton, vgui::Button );
	CModelButton( vgui::Panel *parent, const char *panelname, const char *modelpath );

	virtual void	Paint();
	virtual void	ApplySchemeSettings( vgui::IScheme *scheme );
	virtual void	ApplySettings( KeyValues *inResourceData );

private:
	CModelPanel *	_modelPanel;

	CUtlString		_modelPath;
};

#endif // __VGUI_BITMAPBUTTONTABLE_H__