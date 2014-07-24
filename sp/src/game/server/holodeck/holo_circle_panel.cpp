/*
===============================================================================

	holo_circle_panel.h
		Implements the `holo_circle_panel` entity.

===============================================================================
*/

#include "cbase.h"
#include "base_holo_panel.h"
#include "holo_hand.h"

using namespace holo;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CHoloCirclePanel : public CBaseHoloPanel
{
	enum spawnFlags_t
	{
		SF_PANEL_LOCKED		= ( 1 << 11 ),
		SF_START_OPEN		= ( 1 << 12 ),
		SF_OPEN_CW			= ( 1 << 13 ),
	};

public:
	DECLARE_CLASS( CHoloCirclePanel, CBaseHoloPanel );
	DECLARE_DATADESC();

	// CBaseEntity overrides.
	virtual void	Spawn();

	// CBaseHoloPanel overrides.
	virtual bool	PassesTriggerFilters( CBaseEntity *pOther );

	// CTriggerMultiple overrides.
	virtual void	Touch( CBaseEntity *pOther );
	virtual void	EndTouch( CBaseEntity *pOther );

private:
	// Hammer attributes.
	float			_useTime;				// Length of time (after the gesture is first recognised) the user must 
											// make the circle gesture in order to activate this entity.
	QAngle			_circleNormal;			// Direction of the circle gesture normal required to activate this entity.

	// Hammer inputs.
	void			InputSetUseTime( inputdata_t &inputdata );
	void			InputLock( inputdata_t &inputdata );
	void			InputUnlock( inputdata_t &inputdata );

	// Hammer outputs.
	COutputEvent	_onFullyOpen;			// Fired when the circle gesture has been performed inside the entity for the
											// required time (after the entity was closed), and required direction.
	COutputEvent	_onFullyClosed;			// Fired when the circle gesture has been performed inside the entity for the
											// required time (after the entity was open), and required direction.

	// Save state.
	Vector			_circleDirection;
	bool			_locked;
	bool			_isOpen;
	bool			_openCW;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( holo_circle_panel, CHoloCirclePanel );

BEGIN_DATADESC( CHoloCirclePanel )

	// Save fields.
	DEFINE_FIELD( _locked, FIELD_BOOLEAN ),
	DEFINE_FIELD( _isOpen, FIELD_BOOLEAN ),
	DEFINE_FIELD( _openCW, FIELD_BOOLEAN ),
	DEFINE_FIELD( _circleDirection, FIELD_VECTOR ),

	// Attributes.
	DEFINE_KEYFIELD( _useTime, FIELD_FLOAT, "useTime" ),
	DEFINE_KEYFIELD( _circleNormal, FIELD_VECTOR, "normal" ),

	// Inputs.
	DEFINE_INPUTFUNC( FIELD_VOID, "SetUseTime", InputSetUseTime ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Lock", InputLock ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Unlock", InputUnlock ),

	// Outputs.
	DEFINE_OUTPUT( _onFullyOpen, "OnFullyOpen" ),
	DEFINE_OUTPUT( _onFullyClosed, "OnFullyClosed" ),

END_DATADESC()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloCirclePanel::Spawn()
{
	BaseClass::Spawn();
	_locked = HasSpawnFlags( SF_PANEL_LOCKED );
	_isOpen = HasSpawnFlags( SF_START_OPEN );
	_openCW = HasSpawnFlags( SF_OPEN_CW );

	// Convert the activation angle into a direction vector.
	AngleVectors( _circleNormal, &_circleDirection );
	_circleDirection.NormalizeInPlace();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CHoloCirclePanel::PassesTriggerFilters( CBaseEntity *pOther )
{
	// Ensure we're dealing with the hand entity.
	if( !BaseClass::PassesTriggerFilters( pOther ) )
	{
		return false;
	}

	CHoloHand *pHand = (CHoloHand *)pOther;
	const SFrame &frame = pHand->GetFrame();

	// Obviously the circle gesture must be active for the trigger for fire.
	if( !frame.IsGestureActive( EGesture::GESTURE_CIRCLE ) )
	{
		return false;
	}

	// Ensure the circle gesture is going the correct direction to trigger.
	const SCircleGesture &circle = frame._circle;
	bool panelRequiresClockwise = ( _isOpen != _openCW );
	if( circle.clockwise != panelRequiresClockwise )
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloCirclePanel::Touch( CBaseEntity *pOther )
{
	if( m_bDisabled || !PassesTriggerFilters( pOther ) )
	{
		return;
	}

	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
	CHoloHand *pHand = (CHoloHand *)pOther;
	const SFrame &frame = pHand->GetFrame();
	const SCircleGesture &circle = frame._circle;

	pHand->DebugStartTouch();

	if( circle.duration >= _useTime )
	{
		// The circle gesture has been made long enough.
		// Fire the map event.
		COutputEvent *event = _isOpen ? &_onFullyClosed : &_onFullyOpen;
		event->FireOutput( pPlayer, this );

		_isOpen = !_isOpen;
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloCirclePanel::EndTouch( CBaseEntity *pOther )
{
	CHoloHand *pHand = dynamic_cast<CHoloHand *>( pOther );
	if( !pHand )
	{
		return;
	}

	pHand->DebugEndTouch();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloCirclePanel::InputSetUseTime( inputdata_t &inputdata )
{
	_useTime = inputdata.value.Float();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloCirclePanel::InputLock( inputdata_t &inputdata )
{
	_locked = true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloCirclePanel::InputUnlock( inputdata_t &inputdata )
{
	_locked = false;
}