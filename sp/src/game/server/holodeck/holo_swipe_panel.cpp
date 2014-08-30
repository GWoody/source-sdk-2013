/*
===============================================================================

	holo_swipe_panel.cpp
		Implements the `holo_swipe_panel` entity.

===============================================================================
*/

#include "cbase.h"
#include "base_holo_panel.h"
#include "holo_hand.h"

using namespace holo;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CHoloSwipePanel : public CBaseHoloPanel
{
	enum spawnFlags_t
	{
		SF_PANEL_LOCKED		= ( 1 << 11 ),
	};

public:
	DECLARE_CLASS( CHoloSwipePanel, CBaseHoloPanel );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	// CBaseEntity overrides.
	virtual void	Spawn();

	// CBaseHoloPanel overrides.
	virtual bool	PassesTriggerFilters( CBaseEntity *pOther );

	// CTriggerMultiple overrides.
	virtual void	Touch( CBaseEntity *pOther );
	virtual void	EndTouch( CBaseEntity *pOther );

	// CBaseHoloPanel implementation.
	bool			UsesPanelSprite() const			{ return true; }
	SPanelSprite	GetPanelSprite() const			{ return SPanelSprite( 0.075f, _swipeAngle + QAngle(0, 90, 0), "holodeck/swipe_overlay.vmt" ); }

private:
	float			ActivationDirectionDelta( const Vector &v );

	// Hammer attributes.
	float			_minSpeed;
	QAngle			_swipeAngle;

	// Hammer inputs.
	void			InputLock( inputdata_t &inputdata );
	void			InputUnlock( inputdata_t &inputdata );

	// Hammer outputs.
	COutputEvent	_onSwipe;
	COutputEvent	_onReverseSwipe;

	// Save state.
	bool			_locked;
	Vector			_swipeDirection;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( holo_swipe_panel, CHoloSwipePanel );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
BEGIN_DATADESC( CHoloSwipePanel )

	// Save fields.
	DEFINE_FIELD( _locked, FIELD_BOOLEAN ),
	DEFINE_FIELD( _swipeDirection, FIELD_VECTOR ),

	// Attributes.
	DEFINE_KEYFIELD( _swipeAngle, FIELD_VECTOR, "swipeAngle" ),
	DEFINE_KEYFIELD( _minSpeed, FIELD_FLOAT, "minSpeed" ),

	// Inputs.
	DEFINE_INPUTFUNC( FIELD_VOID, "Lock", InputLock ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Unlock", InputUnlock ),

	// Outputs.
	DEFINE_OUTPUT( _onSwipe, "OnSwipe" ),
	DEFINE_OUTPUT( _onReverseSwipe, "OnReverseSwipe" ),

END_DATADESC()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IMPLEMENT_SERVERCLASS_ST( CHoloSwipePanel, DT_HoloSwipePanel )
END_SEND_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloSwipePanel::Spawn()
{
	BaseClass::Spawn();

	_locked = HasSpawnFlags( SF_PANEL_LOCKED );
	m_bDisabled = false;

	// Convert the activation angle into a direction vector.
	AngleVectors( _swipeAngle, &_swipeDirection );
	_swipeDirection.NormalizeInPlace();

	if( _locked )
	{
		RemoveEntityGlow();
		RemoveAnimation();
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CHoloSwipePanel::PassesTriggerFilters( CBaseEntity *pOther )
{
	// Ensure we're dealing with the hand entity.
	if( !BaseClass::PassesTriggerFilters( pOther ) )
	{
		return false;
	}

	CHoloHand *pHand = (CHoloHand *)pOther;
	const CFrame &frame = pHand->GetFrame();

	if( !frame.IsGestureActive( EGesture::GESTURE_SWIPE ) )
	{
		return false;
	}

	const CSwipeGesture &swipe = frame.GetSwipeGesture();

	//
	// Delta must be within the range (0 ± TOLERANCE) degrees OR the range
	// (180 ± TOLERANCE) degrees in order to trigger the panel.
	//
	{
		const float TOLERANCE = 33.0f;
		float delta = ActivationDirectionDelta( swipe.GetDirection() );
		
		if( delta > TOLERANCE && delta < 180.0f - TOLERANCE )
		{
			return false;
		}

		if( delta > 180.0f + TOLERANCE && delta < 360.0f - TOLERANCE )
		{
			return false;	
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloSwipePanel::Touch( CBaseEntity *pOther )
{
	if( _locked || m_bDisabled || !PassesTriggerFilters( pOther ) )
	{
		return;
	}

	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
	CHoloHand *pHand = (CHoloHand *)pOther;
	const CFrame &frame = pHand->GetFrame();
	const CSwipeGesture &swipe = frame.GetSwipeGesture();

	pHand->DebugStartTouch();

	float delta = ActivationDirectionDelta( swipe.GetDirection() );
	COutputEvent *event = ( delta <= 270 && delta >= 90 ) ? &_onReverseSwipe : &_onSwipe;

	event->FireOutput( pPlayer, this );

	m_bDisabled = true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloSwipePanel::EndTouch( CBaseEntity *pOther )
{
	CHoloHand *pHand = dynamic_cast<CHoloHand *>( pOther );
	if( !pHand )
	{
		return;
	}

	pHand->DebugEndTouch();
	m_bDisabled = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float CHoloSwipePanel::ActivationDirectionDelta( const Vector &v )
{
	const Vector norm = v.Normalized();
	float dot = _swipeDirection.Dot( norm );
	float mag = _swipeDirection.Length() * norm.Length();
	float delta = acos( dot / mag );
	return RAD2DEG( delta );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloSwipePanel::InputLock( inputdata_t &inputdata )
{
	_locked = true;
	RemoveEntityGlow();
	RemoveAnimation();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloSwipePanel::InputUnlock( inputdata_t &inputdata )
{
	_locked = false;
	InitEntityGlow();
	InitAnimation();
}