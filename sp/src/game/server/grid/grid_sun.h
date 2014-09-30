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
	CGridSun();

	virtual void	Activate();

	void			SetPitchYaw( float pitch, float yaw );
	const Vector &	GetLightDirection() const;

private:
	Vector			_lightDirection;
};

#endif // __GRID_SUN_H__