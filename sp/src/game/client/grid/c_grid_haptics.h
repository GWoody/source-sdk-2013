/*
===============================================================================

	c_grid_haptics.h
	Defines the client side Grid haptic class.

===============================================================================
*/

#ifndef __C_GRID_HAPTICS_H__
#define __C_GRID_HAPTICS_H__

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_GridHaptics
{
public:
	DECLARE_CLASS_NOBASE( C_GridHaptics );
	DECLARE_EMBEDDED_NETWORKVAR();

	C_GridHaptics();

private:
	int				_power;
	int				_frequency;
	bool			_enabled;
};

EXTERN_RECV_TABLE( DT_GridHaptics );

#endif // __C_GRID_HAPTICS_H__