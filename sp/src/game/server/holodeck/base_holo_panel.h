/*
===============================================================================

	base_holo_panel.h
		Implements common Holodeck panel behaviour.

===============================================================================
*/

#ifndef __BASE_HOLO_PANEL_H__
#define __BASE_HOLO_PANEL_H__

#include "triggers.h"

class CSpriteOriented;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CBaseHoloPanel : public CTriggerMultiple
{
public:
	DECLARE_CLASS( CBaseHoloPanel, CTriggerMultiple );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	// CBaseEntity overrides.
	virtual void	Spawn();
	virtual void	UpdateOnRemove();
	virtual int		UpdateTransmitState();

	// CTriggerMultiple overrides.
	virtual bool	PassesTriggerFilters( CBaseEntity *pOther );

	// Animation helpers.
	virtual bool	UsesAnimatedSprite() const = 0;
	virtual float	GetAnimatedSpriteScale() const = 0;
	virtual QAngle	GetAnimatedSpriteAngles() const = 0;
	virtual const char *	GetAnimatedSpritePath() const = 0;

protected:
	CNetworkHandle( CSpriteOriented, _animation );

private:
	void			InitEntityGlow();
	void			RemoveEntityGlow();

	void			InitAnimation();
	void			RemoveAnimation();

	// Save state.
	EHANDLE			_glowTarget;

	// Hammer attributes.
	string_t		_glowTargetName;
};

#endif // __BASE_HOLO_PANEL_H__