/*
===============================================================================

	grid_proptool.h
	Implements the estate prop spawning tool.

===============================================================================
*/

#ifndef __GRID_PROPTOOL_H__
#define __GRID_PROPTOOL_H__

#include "grid_base_weapon.h"
#include "props.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridPropTool : public CGridBaseWeapon
{
public:
	DECLARE_CLASS( CGridPropTool, CGridBaseWeapon );

	CGridPropTool();

	void			SetProp( const char *prop )				{ _prop.Set( prop ); }
	virtual bool	IsPropTool() const						{ return true; }

	virtual void	TakeOut( EHand hand );
	virtual void	PutAway();
	virtual void	ItemPreFrame();

	// Shooting.
	virtual void	ShootSingleBullet();

private:
	CHandle<CDynamicProp>	_preview;
	CUtlString		_prop;

	int				_handIdx;
};

#endif // __GRID_PROPTOOL_H__