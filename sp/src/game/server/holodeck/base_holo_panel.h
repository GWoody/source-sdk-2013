/*
===============================================================================

	base_holo_panel.h
		Implements common Holodeck panel behaviour.

===============================================================================
*/

#ifndef __BASE_HOLO_PANEL_H__
#define __BASE_HOLO_PANEL_H__

#include "triggers.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CBaseHoloPanel : public CTriggerMultiple
{
public:
	DECLARE_CLASS( CBaseHoloPanel, CTriggerMultiple );
	DECLARE_DATADESC();

	// CBaseEntity overrides.
	virtual void	Spawn();
	virtual void	UpdateOnRemove();

	// CTriggerMultiple overrides.
	virtual bool	PassesTriggerFilters( CBaseEntity *pOther );

private:
	void			InitEntityGlow();
	void			RemoveEntityGlow();

	// Save state.
	EHANDLE			_glowTarget;

	// Hammer attributes.
	string_t		_glowTargetName;
};

#endif // __BASE_HOLO_PANEL_H__