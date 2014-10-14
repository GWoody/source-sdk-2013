/*
===============================================================================

	holo_player.h
	Implements functionality specific for the Holodeck player.

===============================================================================
*/

#ifndef __HOLO_PLAYER_H__
#define __HOLO_PLAYER_H__

#include "player.h"
#include "holo_hand.h"
#include "holo_world_screen_manager.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CHoloPlayer : public CBasePlayer
{
public:
	DECLARE_CLASS( CHoloPlayer, CBasePlayer );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	CHoloPlayer();

	// CBasePlayer overrides.
	virtual void	Spawn();
	virtual void	Think();
	virtual void	Event_Killed( const CTakeDamageInfo &info );
	virtual bool	IsUseableEntity( CBaseEntity *pEntity, unsigned int requiredCaps );

	virtual void	PlayerUse();
	virtual float	GetHeldObjectMass( IPhysicsObject *pHeldObject );

	// Weapon overrides.
	virtual bool	BumpWeapon( CBaseCombatWeapon *pWeapon );

	// UserCmd processing.
	virtual void	ProcessUsercmds( CUserCmd *cmds, int numcmds, int totalcmds, int dropped_packets, bool paused );

	// Accessors.
	CHoloHand *		GetHandEntity( EHand hand ) const	{ return (CHoloHand *)_hands[hand].Get(); }
	Vector			GetHeadOffset() const				{ return _viewoffset; }
	CHoloWorldScreenManager &	GetScreenManager()		{ return _screenManager; }

protected:
	virtual void	ProcessFrame( const CFrame &frame );
	virtual void	OnInvalidFrame();

	virtual bool	CanAttemptPickup() const			{ return true; }

private:
	// Custom gesture detection.
	void			HandlePickupGesture( const CFrame &frame );
	void			HandleScreenGesture( const CFrame &frame );

	// UserCmd processing.
	CFrame	AccumulateHoloFrame( CUserCmd *cmds, int numcmds, int totalcmds, int dropped_packets, bool paused );

	CNetworkArray( EHANDLE, _hands, HAND_COUNT );				// The hand entity which is used to interact with the environment.
	CNetworkVar( Vector, _viewoffset );

	CHoloWorldScreenManager	_screenManager;

	int				_invalidFrames;
	CFrame			_lastValidFrame;
};

#endif // __HOLO_PLAYER_H__