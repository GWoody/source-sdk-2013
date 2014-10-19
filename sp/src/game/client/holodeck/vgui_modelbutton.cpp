/*
===============================================================================

	vgui_BitmapButtonTable.cpp
	Implements a control that arranges bitmap buttons into a table.

===============================================================================
*/

#include "cbase.h"
#include "vgui_modelbutton.h"
using namespace vgui;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CModelButton::CModelButton( Panel *parent, const char *panelname, const char *modelPath ) : 
	BaseClass( parent, panelname, "button" )
{
	_modelPath.Set( modelPath );
	_modelPanel = NULL;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CModelButton::Paint()
{
	if( !_modelPanel )
	{
		return;
	}

	BaseClass::Paint();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CModelButton::ApplySchemeSettings( IScheme *scheme )
{
	BaseClass::ApplySchemeSettings( scheme );

	KeyValues *kv = new KeyValues( "model" );
	kv->SetString( "modelname", _modelPath.Get() );
	kv->SetString( "modelname_hwm", "" );
	kv->SetInt( "skin", 0 );
	kv->SetInt( "spotlight", 1 );

	_modelPanel = new CModelPanel( this, "model" );
	_modelPanel->ParseModelInfo( kv );
	kv->deleteThis();

	int wide, tall;
	GetSize( wide, tall );
	_modelPanel->SetSize( wide, tall );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CModelButton::ApplySettings( KeyValues *inResourceData )
{
	BaseClass::ApplySettings( inResourceData );
}