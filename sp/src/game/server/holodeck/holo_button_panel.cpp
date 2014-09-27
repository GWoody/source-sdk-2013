/*
===============================================================================

	holo_button_panel.cpp
		Implements the `holo_button_panel` entity.

===============================================================================
*/

#include "cbase.h"
#include "holo_base_panel.h"
#include "holo_hand.h"
#include "grid/grid_player.h"
#include "holo_haptic_events.h"

using namespace holo;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CHoloButtonPanel : public CBaseHoloPanel
{
	enum spawnFlags_t
	{
		SF_BUTTON_LOCKED = ( 1 << 11 ),
	};

public:
	DECLARE_CLASS( CHoloButtonPanel, CBaseHoloPanel );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	// CBaseEntity overrides.
	virtual void	Spawn();
	virtual void	Precache();

	// CBaseHoloPanel overrides.
	virtual bool	PassesTriggerFilters( CBaseEntity *pOther );

	// CTriggerMultiple overrides.
	virtual void	Touch( CBaseEntity *pOther );
	virtual void	EndTouch( CBaseEntity *pOther );

	// CBaseHoloPanel implementation.
	bool			UsesPanelSprite() const			{ return true; }
	SPanelSprite	GetPanelSprite() const			{ return SPanelSprite( 0.1f, _activationAngle, "holodeck/tap_overlay.vmt" ); }

private:
	float			ActivationDirectionDelta( const Vector &v );

	// Hammer attributes.
	QAngle			_activationAngle;		// Direction of the Leap Motion press gesture required to activate this entity.
	string_t		_pressSound;			// Name of the sound to play when the button is pressed.
	string_t		_lockedSound;			// Name of the sound to play when the button is pressed (when locked).
	float			_volume;				// Volume modifier of the sound to play when the button is pressed.

	// Hammer inputs.
	void			InputLock( inputdata_t &inputdata );
	void			InputUnlock( inputdata_t &inputdata );

	// Hammer outputs.
	COutputEvent	_onPress;				// Fired when a valid Leap Motion press gesture was performed.
	COutputEvent	_onPressLocked;			// Fired when a button has been locked by the map designer.

	// Save state.
	bool			_locked;
	Vector			_activationDirection;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( holo_button_panel, CHoloButtonPanel );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
BEGIN_DATADESC( CHoloButtonPanel )

	// Save fields.
	DEFINE_FIELD( _locked, FIELD_BOOLEAN ),
	DEFINE_FIELD( _activationDirection, FIELD_VECTOR ),

	// Attributes.
	DEFINE_KEYFIELD( _pressSound, FIELD_SOUNDNAME, "pressSound" ),
	DEFINE_KEYFIELD( _lockedSound, FIELD_SOUNDNAME, "lockedSound" ),
	DEFINE_KEYFIELD( _activationAngle, FIELD_VECTOR, "pressDirection" ),
	DEFINE_KEYFIELD( _volume, FIELD_FLOAT, "volume" ),

	// Inputs.
	DEFINE_INPUTFUNC( FIELD_VOID, "Lock", InputLock ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Unlock", InputUnlock ),

	// Outputs.
	DEFINE_OUTPUT( _onPress, "OnPress" ),
	DEFINE_OUTPUT( _onPressLocked, "OnPressLocked" ),

END_DATADESC()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
IMPLEMENT_SERVERCLASS_ST( CHoloButtonPanel, DT_HoloButtonPanel )
END_SEND_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloButtonPanel::Spawn()
{
	Precache();

	BaseClass::Spawn();

	_locked = HasSpawnFlags( SF_BUTTON_LOCKED );
	_volume = clamp( _volume, 0.0f, 1.0f );

	// Convert the activation angle into a direction vector.
	AngleVectors( _activationAngle, &_activationDirection );
	_activationDirection.NormalizeInPlace();

	if( _locked )
	{
		RemoveEntityGlow();
		RemoveAnimation();
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloButtonPanel::Precache()
{
	if( _pressSound != NULL_STRING )
	{
		PrecacheScriptSound( STRING(_pressSound) );
	}

	if( _lockedSound != NULL_STRING )
	{
		PrecacheScriptSound( STRING(_lockedSound) );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CHoloButtonPanel::PassesTriggerFilters( CBaseEntity *pOther )
{
	// Ensure we're dealing with the hand entity.
	if( !BaseClass::PassesTriggerFilters( pOther ) )
	{
		return false;
	}

	// Access required structures.
	const CHoloHand *pHand = (const CHoloHand *)pOther;
	const CFinger &pointer = pHand->GetHoloHand().GetFingerByType( FINGER_POINTER );

	const Vector normalizedDirection = pointer.GetDirection().Normalized();
	const Vector normalizedVelocity = pointer.GetTipVelocity().Normalized();

	// Ensure the finger and movement is pointing towards the button.
	const float TOLERANCE = 33.0f;
	float directionDelta = ActivationDirectionDelta( normalizedDirection );
	float velocityDelta = ActivationDirectionDelta( normalizedVelocity );
	if( directionDelta < TOLERANCE && velocityDelta < TOLERANCE )
	{
		// The finger and button angles can differ by up to 33 degrees and still be accepted.
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloButtonPanel::Touch( CBaseEntity *pOther )
{
	if( m_bDisabled || !PassesTriggerFilters( pOther ) )
	{
		return;
	}

	CHoloHand *hand = (CHoloHand *)pOther;
	CGridPlayer *pPlayer = (CGridPlayer *)hand->GetOwnerPlayer();

	//
	// All conditions are satisfied. Trigger the map event.
	//
	COutputEvent *event = _locked ? &_onPressLocked : &_onPress;
	event->FireOutput( pPlayer, this );

	//
	// Play the press sound.
	//
	string_t *sound = _locked ? &_lockedSound : &_pressSound;
	const char *pszSound = sound->ToCStr();
	if( pszSound )
	{
		UTIL_EmitAmbientSound( entindex( ), GetAbsOrigin(), pszSound, _volume, SNDLVL_NORM, SND_NOFLAGS, PITCH_NORM );
	}

	hand->DebugStartTouch();

	pPlayer->GetHaptics().PushEvent( new CButtonPressHapticEvent );

	//
	// We can't fire the button again until the hand has left the trigger.
	//
	m_bDisabled = true;
}

//-----------------------------------------------------------------------------
// Readies the button for another press.
//-----------------------------------------------------------------------------
void CHoloButtonPanel::EndTouch( CBaseEntity *pOther )
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
float CHoloButtonPanel::ActivationDirectionDelta( const Vector &v )
{
	const Vector norm = v.Normalized();
	float dot = _activationDirection.Dot( norm );
	float mag = _activationDirection.Length() * norm.Length();
	float delta = acos( dot / mag );
	return RAD2DEG( delta );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloButtonPanel::InputLock( inputdata_t &inputdata )
{
	_locked = true;
	RemoveEntityGlow();
	RemoveAnimation();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloButtonPanel::InputUnlock( inputdata_t &inputdata )
{
	_locked = false;
	InitEntityGlow();
	InitAnimation();
}