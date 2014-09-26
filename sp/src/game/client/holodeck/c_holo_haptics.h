/*
===============================================================================

	c_holo_haptics.h
	Defines the client side Holodeck haptic class.

===============================================================================
*/

#ifndef __C_HOLO_HAPTICS_H__
#define __C_HOLO_HAPTICS_H__

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_HoloHaptics
{
public:
	DECLARE_CLASS_NOBASE( C_HoloHaptics );
	DECLARE_EMBEDDED_NETWORKVAR();

	C_HoloHaptics();

	int				GetPower() const		{ return _power; }
	int				GetFrequency() const	{ return _frequency; }
	bool			IsEnabled() const		{ return _enabled; }

	void			Update();

private:
	int				_power;
	int				_frequency;
	bool			_enabled;
};

EXTERN_RECV_TABLE( DT_HoloHaptics );

#endif // __C_HOLO_HAPTICS_H__