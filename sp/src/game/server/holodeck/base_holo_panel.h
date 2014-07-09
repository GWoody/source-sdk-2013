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

	// CBaseEntity overrides.
	virtual void	Spawn();

	// CTriggerMultiple overrides.
	virtual bool	PassesTriggerFilters( CBaseEntity *pOther );
};

#endif // __BASE_HOLO_PANEL_H__