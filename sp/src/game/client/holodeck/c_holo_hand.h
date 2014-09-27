/*
===============================================================================

	c_holo_hand.h
	Client side implementation of the Holodeck hand entity.

===============================================================================
*/

#ifndef __C_HOLO_HAND_H__
#define __C_HOLO_HAND_H__

#include "c_baseentity.h"
#include "c_holo_haptics.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class C_HoloHand : public C_BaseEntity
{
public:
	DECLARE_CLASS( C_HoloHand, C_BaseEntity );
	DECLARE_CLIENTCLASS();

	// C_BaseEntity overrides.
	virtual void	Simulate();
	virtual void	PostDataUpdate( DataUpdateType_t updateType );

private:
	C_HoloHaptics	_haptics;
};

#endif // __C_HOLO_HAND_H__