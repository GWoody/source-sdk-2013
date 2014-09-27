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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CHoloHapticProxy : public CPointEntity
{
public:
	DECLARE_CLASS( CHoloHapticProxy, CPointEntity );
	DECLARE_DATADESC();

	CHoloHapticProxy();

private:
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

	// Hammer attributes.
	float			_startPower, _endPower;
	float			_startFreq, _endFreq;
	CProxyHapticEvent::ELerp	_lerpType;
	float			_time;
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

	// Attributes.
	DEFINE_KEYFIELD( _startPower, FIELD_FLOAT, "startPower" ),
	DEFINE_KEYFIELD( _endPower, FIELD_FLOAT, "endPower" ),
	DEFINE_KEYFIELD( _startFreq, FIELD_FLOAT, "startFreq" ),
	DEFINE_KEYFIELD( _endFreq, FIELD_FLOAT, "endFreq" ),
	DEFINE_KEYFIELD( _lerpType, FIELD_INTEGER, "lerpType" ),
	DEFINE_KEYFIELD( _time, FIELD_FLOAT, "time" ),

END_DATADESC()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CHoloHapticProxy::CHoloHapticProxy()
{
	_startPower = _endPower = 0.0f;
	_startFreq = _endFreq = 0.0f;

	_lerpType = CProxyHapticEvent::ELerp::LINEAR;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputLerp( inputdata_t &data )
{
	CHoloPlayer *player = dynamic_cast<CHoloPlayer *>( UTIL_GetLocalPlayer() );
	if( !player )
	{
		ConColorMsg( COLOR_YELLOW, __FUNCTION__": failed to get pointer to local player!\n" );
		return;
	}

	CProxyHapticEvent *event = new CProxyHapticEvent( _startPower, _endPower, _startFreq, _endFreq, _time, _lerpType );
	player->GetHaptics().PushEvent( event );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputSetLinearLerp( inputdata_t &data )
{
	_lerpType = CProxyHapticEvent::ELerp::LINEAR;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputSetExpLerp( inputdata_t &data )
{
	_lerpType = CProxyHapticEvent::ELerp::EXPONENTIAL;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputSetSinLerp( inputdata_t &data )
{
	_lerpType = CProxyHapticEvent::ELerp::SIN_CURVE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputSetLerpTime( inputdata_t &data )
{
	_time = max( 0.0f, data.value.Float() );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputSetStartPower( inputdata_t &data )
{
	_startPower = clamp( data.value.Float(), 0.0f, 1.0f );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputSetEndPower( inputdata_t &data )
{
	_endPower = clamp( data.value.Float(), 0.0f, 1.0f );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputSetStartFreq( inputdata_t &data )
{
	_startFreq = clamp( data.value.Float(), 0.0f, 1.0f );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloHapticProxy::InputSetEndFreq( inputdata_t &data )
{
	_endFreq = clamp( data.value.Float(), 0.0f, 1.0f );
}