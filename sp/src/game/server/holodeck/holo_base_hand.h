/*
===============================================================================

	holo_base_hand.h
		Implements the hand entity.
		The hand entity mirrors the client side Leap Motion hand state.

===============================================================================
*/

#ifndef __HOLO_BASE_HAND_H__
#define __HOLO_BASE_HAND_H__

#include "baseentity.h"
#include "holodeck/holo_shared.h"
#include "holo_frame_filter.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CBaseHoloHand : public CBaseEntity
{
public:
	DECLARE_CLASS( CBaseHoloHand, CBaseEntity );
					CBaseHoloHand();

	// CBaseEntity overrides.
	virtual void	Spawn();
	virtual void	Precache();
	virtual	bool	CreateVPhysics();
	virtual void	Think();

	// Mutators.
	void			SetType( holo::EHand type )		{ _type = type; }

	// Accessors.
	const holo::CFrame &	GetFrame() const;
	const holo::CHand &	GetHoloHand() const;
	const holo::EHand	GetType() const				{ return _type; }
	CBasePlayer *	GetOwnerPlayer() const			{ return (CBasePlayer *)GetOwnerEntity(); }

	// Held object.
	void			SetUseEntity( CBaseEntity *entity );
	CBaseEntity *	GetUseEntity();
	bool			ClearUseEntity();

	// Object interaction.
	void			PickupObject( CBaseEntity *pObject, bool bLimitMassAndSize = true );
	bool			IsHoldingEntity( CBaseEntity *pEnt );
	float			GetHeldObjectMass( IPhysicsObject *pHeldObject );
	void			AttemptObjectPickup();
	void			AttemptObjectDrop();

	// Frame processing.
	void			ProcessFrame( const holo::CFrame &frame );

	void			SetInvisible( bool invisible );

	void			DebugStartTouch();
	void			DebugEndTouch();

private:
	CBaseEntity *	FindUseEntity();
	bool			CanPickupObject( CBaseEntity *pObject, float massLimit, float sizeLimit );
	float			IntervalDistance( float x, float x0, float x1 );

	void			RenderDebugHand();
	Vector			GetOriginOffset() const;

	holo::CFrame	_transformedFrame, _untransformedFrame;

	holo::CFrameFilter _filter;
	holo::EHand		_type;

	Vector			_lastOriginDelta;

	// Object interaction.
	EHANDLE			_heldEntity;
};

#endif // __HOLO_BASE_HAND_H__