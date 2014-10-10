/*
===============================================================================

	holo_trigger_haptic.h
	Defines a trigger volume that fires up the users etactors.

===============================================================================
*/

#ifndef __HOLO_TRIGGER_HAPTIC_H__
#define __HOLO_TRIGGER_HAPTIC_H__

#include "triggers.h"

class CTriggerHapticEvent;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CHoloTriggerHaptic : public CTriggerMultiple
{
public:
	DECLARE_CLASS( CHoloTriggerHaptic, CTriggerMultiple );
	DECLARE_DATADESC();

	CHoloTriggerHaptic();

	// CTriggerMultiple overrides.
	virtual bool	PassesTriggerFilters( CBaseEntity *pOther );
	virtual void	Touch( CBaseEntity *pOther );
	virtual void	EndTouch( CBaseEntity *pOther );

private:
	int				_power;
	int				_frequency;

	CUtlMap<int, CTriggerHapticEvent *> _events;
};

#endif // __HOLO_TRIGGER_HAPTIC_H__