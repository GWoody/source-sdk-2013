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
struct SPanelSprite
{
	// Default constructor.
	SPanelSprite():
		scale( 0.0f ), angle( 0.0f, 0.0f, 0.0f ), path( "" )
	{
	}

	// Initialisation constructor.
	SPanelSprite( float _scale, const QAngle &_angle, const CUtlString &_path ) :
		scale( _scale ), angle( _angle ), path( _path )
	{
	}

	float			scale;
	QAngle			angle;
	CUtlString		path;
};

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
	virtual bool	UsesPanelSprite() const = 0;
	virtual SPanelSprite	GetPanelSprite() const = 0;

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