/*
===============================================================================

	grid_sun.h
	Extends the base Source Engine sun with real time movement.

===============================================================================
*/

#ifndef __GRID_SUN_H__
#define __GRID_SUN_H__

#include "sun.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridSun : public CSun
{
public:
	DECLARE_CLASS( CGridSun, CSun );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();
	CGridSun();

	// CBaseEntity overrides.
	virtual void	Activate();

	// Mutators.
	void			SetPitchYaw( float pitch, float yaw );
	void			SetFarZ( bool farz );
	void			SetMaterial( const char *material );

	// Accessors.
	const Vector &	GetLightDirection() const;

private:
	Vector			_lightDirection;

	CNetworkVar( bool, _farZ );
};

#endif // __GRID_SUN_H__