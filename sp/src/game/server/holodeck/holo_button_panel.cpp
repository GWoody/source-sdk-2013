/*
===============================================================================

	holo_button_panel.cpp
		Implements the `holo_button_panel` entity.

===============================================================================
*/

#include "cbase.h"
#include "base_holo_panel.h"
#include "holo_hand.h"

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
	DECLARE_DATADESC();

	// CBaseEntity overrides.
	virtual void	Spawn();
	virtual void	Precache();

	// CBaseHoloPanel overrides.
	virtual bool	PassesTriggerFilters( CBaseEntity *pOther );

	// CTriggerMultiple overrides.
	virtual void	Touch( CBaseEntity *pOther );
	virtual void	EndTouch( CBaseEntity *pOther );

private:
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

LINK_ENTITY_TO_CLASS( holo_button_panel, CHoloButtonPanel );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
BEGIN_DATADESC( CHoloButtonPanel )

	// Save fields.
	DEFINE_FIELD( _locked, FIELD_BOOLEAN ),

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
void CHoloButtonPanel::Spawn()
{
	BaseClass::Spawn();
	_locked = HasSpawnFlags( SF_BUTTON_LOCKED );
	_volume = clamp( _volume, 0.0f, 1.0f );

	// Convert the activation angle into a direction vector.
	AngleVectors( _activationAngle, &_activationDirection );
	_activationDirection.NormalizeInPlace();
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

	CHoloHand *pHand = (CHoloHand *)pOther;

	// Ensure the finger is facing the correct direction.
	const SFinger &pointer = pHand->GetFinger( NFinger::FINGER_POINTER );
	const SHand &hand = pHand->GetHand();
	const Vector normalizedFinger = ( pointer.tipPosition - hand.palmPosition ).Normalized();

	float dot = _activationDirection.Dot( normalizedFinger );
	float mag = _activationDirection.Length() * normalizedFinger.Length();
	float angleBetween = acos( dot / mag );
	angleBetween = RAD2DEG( angleBetween );

	if( angleBetween < 33.0f )
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

	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();

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

	CHoloHand *pHand = (CHoloHand *)pOther;
	pHand->DebugStartTouch();

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
	if( !PassesTriggerFilters( pOther ) )
	{
		return;
	}

	CHoloHand *pHand = (CHoloHand *)pOther;
	pHand->DebugEndTouch();

	m_bDisabled = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloButtonPanel::InputLock( inputdata_t &inputdata )
{
	_locked = true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloButtonPanel::InputUnlock( inputdata_t &inputdata )
{
	_locked = false;
}