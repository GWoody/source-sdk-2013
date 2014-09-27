/*
===============================================================================

	holo_haptic_proxy.cpp
	Implements the `holo_haptic_proxy` entity which gives designers control
	over the haptic interface.

===============================================================================
*/

#include "cbase.h"
#include "holo_haptic_events.h"
#include "holo_player.h"
#include "holodeck/holo_shared.h"

using namespace holo;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CHoloHapticProxy : public CPointEntity
{
	enum ETarget
	{
		SF_LEFT_HAND,
		SF_RIGHT_HAND,
	};

	enum ETargetBits
	{
		SF_LEFT_HAND_BIT = ( 1 << SF_LEFT_HAND ),
		SF_RIGHT_HAND_BIT = ( 1 << SF_RIGHT_HAND ),
	};

public:
	DECLARE_CLASS( CHoloHapticProxy, CPointEntity );
	DECLARE_DATADESC();

	CHoloHapticProxy();
	
	virtual void	Spawn();
	virtual void	Think();
	virtual bool	KeyValue( const char *szKeyName, const char *szValue );
	virtual void	Remove();

private:
	void			AddEventToTargetLimbs( CHoloPlayer *player );
	void			RemoveEventFromTargetLimbs( CHoloPlayer *player );

	// Hammer inputs.
	void			InputLerp( inputdata_t &data );
	void			InputSetLinearLerp( inputdata_t &data );
	void			InputSetExpLerp( inputdata_t &data );
	void			InputSetSinLerp( inputdata_t &data );
	void			InputSetLerpTime( inputdata_t &data );
	void			InputSetStartPower( inputdata_t &data );
	void			InputSetEndPower( inputdata_t &data );
	void			InputSetStartFreq( inputdata_t &data );
	void			InputSetEndFreq( inputdata_t &data );
	void			InputSwapStartEnd( inputdata_t &data );
	void			InputEnable( inputdata_t &data );
	void			InputDisable( inputdata_t &data );

	CProxyHapticEvent *	_event;
};

//-----------------------------------------------------------------------------
// Source entity configuration.
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( holo_haptic_proxy, CHoloHapticProxy );

BEGIN_DATADESC( CHoloHapticProxy )

	// Inputs.
	DEFINE_INPUTFUNC( FIELD_VOID, "Lerp", InputLerp ),
	DEFINE_INPUTFUNC( FIELD_VOID, "SetLinearLerp", InputSetLinearLerp ),
	DEFINE_INPUTFUNC( FIELD_VOID, "SetExpLerp", InputSetExpLerp ),
	DEFINE_INPUTFUNC( FIELD_VOID, "SetSinLerp", InputSetSinLerp ),
	DEFINE_INPUTFUNC( FIELD_FLOAT, "SetLerpTime", InputSetLerpTime ),
	DEFINE_INPUTFUNC( FIELD_FLOAT, "SetStartPower", InputSetStartPower ),
	DEFINE_INPUTFUNC( FIELD_FLOAT, "SetEndPower", InputSetEndPower ),
	DEFINE_INPUTFUNC( FIELD_FLOAT, "SetStartFreq", InputSetStartFreq ),
	DEFINE_INPUTFUNC( FIELD_FLOAT, "SetEndFreq", InputSetEndFreq ),
	DEFINE_INPUTFUNC( FIELD_VOID, "SwapStartEnd", InputSwapStartEnd ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),

END_DATADESC()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CHoloHapticProxy::CHoloHapticProxy()
{
	_event = new CProxyHapticEvent();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::Spawn()
{
	BaseClass::Spawn();

	SetNextThink( gpGlobals->curtime + 0.1f );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::Think()
{
	CHoloPlayer *player = dynamic_cast<CHoloPlayer *>( UTIL_GetLocalPlayer() );
	if( !player )
	{
		ConColorMsg( COLOR_YELLOW, __FUNCTION__": failed to get pointer to local player!\n" );
	}
	else
	{
		AddEventToTargetLimbs( player );
	}

	BaseClass::Think();
}

//-----------------------------------------------------------------------------
// We load all data from Hammer directly into the haptic event object.
//-----------------------------------------------------------------------------
bool CHoloHapticProxy::KeyValue( const char *szKeyName, const char *szValue )
{
	if( FStrEq( szKeyName, "startPower" ) )
	{
		float p = clamp( Q_atof( szValue ), 0.0f, 1.0f );
		_event->SetStartPower( p );
		return true;
	}
	else if( FStrEq( szKeyName, "endPower" ) )
	{
		float p = clamp( Q_atof( szValue ), 0.0f, 1.0f );
		_event->SetEndPower( p );
		return true;
	}
	else if( FStrEq( szKeyName, "startFreq" ) )
	{
		float f = clamp( Q_atof( szValue ), 0.0f, 1.0f );
		_event->SetStartFreq( f );
		return true;
	}
	else if( FStrEq( szKeyName, "endFreq" ) )
	{
		float f = clamp( Q_atof( szValue ), 0.0f, 1.0f );
		_event->SetEndFreq( f );
		return true;
	}
	else if( FStrEq( szKeyName, "lerpType" ) )
	{
		CProxyHapticEvent::ELerp l = (CProxyHapticEvent::ELerp)clamp( Q_atoi( szValue ), 0, CProxyHapticEvent::LERP_COUNT - 1 );
		_event->SetLerpType( l );
		return true;
	}
	else if( FStrEq( szKeyName, "time" ) )
	{
		float t = max( 0.0f, Q_atof( szValue ) );
		_event->SetLerpTime( t );
		return true;
	}
	else if( FStrEq( szKeyName, "holdEnd" ) )
	{
		_event->AddFlag( CProxyHapticEvent::EFlagBit::FL_HOLD_OUT_BIT );
		return true;
	}

	return BaseClass::KeyValue( szKeyName, szValue );
}	

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::Remove()
{
	CHoloPlayer *player = dynamic_cast<CHoloPlayer *>( UTIL_GetLocalPlayer() );
	if( !player )
	{
		ConColorMsg( COLOR_YELLOW, __FUNCTION__": failed to get pointer to local player!\n" );
		return;
	}

	RemoveEventFromTargetLimbs( player );

	delete _event;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::AddEventToTargetLimbs( CHoloPlayer *player )
{
	if( GetSpawnFlags() & SF_LEFT_HAND_BIT )
	{
		player->GetHandEntity( EHand::LEFT )->GetHaptics().AddEvent( _event );
	}

	if( GetSpawnFlags() & SF_RIGHT_HAND_BIT )
	{
		player->GetHandEntity( EHand::RIGHT )->GetHaptics().AddEvent( _event );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::RemoveEventFromTargetLimbs( CHoloPlayer *player )
{
	if( GetSpawnFlags() & SF_LEFT_HAND_BIT )
	{
		player->GetHandEntity( EHand::LEFT )->GetHaptics().RemoveEvent( _event );
	}

	if( GetSpawnFlags() & SF_RIGHT_HAND_BIT )
	{
		player->GetHandEntity( EHand::RIGHT )->GetHaptics().RemoveEvent( _event );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputLerp( inputdata_t &data )
{
	_event->StartLerping();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputSetLinearLerp( inputdata_t &data )
{
	_event->SetLerpType( CProxyHapticEvent::ELerp::LINEAR );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputSetExpLerp( inputdata_t &data )
{
	_event->SetLerpType( CProxyHapticEvent::ELerp::EXPONENTIAL );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputSetSinLerp( inputdata_t &data )
{
	_event->SetLerpType( CProxyHapticEvent::ELerp::SIN_CURVE );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputSetLerpTime( inputdata_t &data )
{
	float t = max( 0.0f, data.value.Float() );
	_event->SetLerpTime( t );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputSetStartPower( inputdata_t &data )
{
	float p = clamp( data.value.Float(), 0.0f, 1.0f );
	_event->SetStartPower( p );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputSetEndPower( inputdata_t &data )
{
	float p = clamp( data.value.Float(), 0.0f, 1.0f );
	_event->SetEndPower( p );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputSetStartFreq( inputdata_t &data )
{
	float f = clamp( data.value.Float(), 0.0f, 1.0f );
	_event->SetStartFreq( f );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputSetEndFreq( inputdata_t &data )
{
	float f = clamp( data.value.Float(), 0.0f, 1.0f );
	_event->SetEndFreq( f );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputSwapStartEnd( inputdata_t &data )
{
	_event->SwapStartEnd();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputEnable( inputdata_t &data )
{
	_event->Enable();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputDisable( inputdata_t &data )
{
	_event->Disable();
}