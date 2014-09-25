/*
===============================================================================

	grid_haptics.h
	Defines the interface for the Grid haptic class.

===============================================================================
*/

#ifndef __GRID_HAPTICS_H__
#define __GRID_HAPTICS_H__

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CGridHaptics
{
public:
	DECLARE_SIMPLE_DATADESC();
	DECLARE_CLASS_NOBASE( CGridHaptics );
	DECLARE_EMBEDDED_NETWORKVAR();

	CGridHaptics();

	void			SetPower( unsigned char power )			{ _power = power; }
	void			SetFrequency( unsigned char freq )		{ _frequency = freq; }
	void			SetEnabled( bool enabled )				{ _enabled = enabled; }

private:
	CNetworkVar( int, _power );
	CNetworkVar( int, _frequency );
	CNetworkVar( bool, _enabled );
};

EXTERN_SEND_TABLE( DT_GridHaptics );

#endif // __GRID_HAPTICS_H__