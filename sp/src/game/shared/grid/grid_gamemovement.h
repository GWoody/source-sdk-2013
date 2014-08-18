/*
===============================================================================

	grid_gamemovement.h
	Mostly borrowed from `hl_gamemovement.h`

===============================================================================
*/

#ifndef __GRID_GAMEMOVEMENT_H__
#define __GRID_GAMEMOVEMENT_H__

#include "gamemovement.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridGameMovement : public CGameMovement
{
	typedef CGameMovement BaseClass;
	
public:
	CGridGameMovement();
};

#endif // __GRID_GAMEMOVEMENT_H__