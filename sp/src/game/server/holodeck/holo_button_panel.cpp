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

	// CBaseHoloPanel overrides.
	virtual bool	PassesTriggerFilters( CBaseEntity *pOther );

	// CTriggerMultiple overrides.
	virtual void	Touch( CBaseEntity *pOther );
	virtual void	EndTouch( CBaseEntity *pOther );

private:
	// Hammer attributes.
	Vector			_direction;				// Direction of the Leap Motion press gesture required to activate this entity.
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
	DEFINE_KEYFIELD( _direction, FIELD_VECTOR, "pressDirection" ),
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

	_direction.NormalizeInPlace();
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
	const SFinger &pointer = pHand->GetFinger( holo::FINGER_POINTER );
	const Vector normalizedFinger = pointer.direction.Normalized();

	if( _direction.Dot( normalizedFinger ) > 0.75 )
	{
		// The finger and button angles can differ by 22.5 degrees and still be accepted.
		return true;
	}

	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoloButtonPanel::Touch( CBaseEntity *pOther )
{
	if( !PassesTriggerFilters( pOther ) )
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