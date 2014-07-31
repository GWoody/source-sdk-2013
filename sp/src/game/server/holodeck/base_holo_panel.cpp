/*
===============================================================================

	base_holo_panel.h
		Implements common Holodeck panel behaviour.

===============================================================================
*/

#include "cbase.h"
#include "base_holo_panel.h"
#include "sprite.h"

//-----------------------------------------------------------------------------
// Specify the fields to load from the BSP, and the fields to write to the save.
//-----------------------------------------------------------------------------
BEGIN_DATADESC( CBaseHoloPanel )

	// Save state.
	DEFINE_FIELD( _glowTarget, FIELD_EHANDLE ),
	DEFINE_FIELD( _animation, FIELD_EHANDLE ),

	// Hammer attributes.
	DEFINE_KEYFIELD( _glowTargetName, FIELD_STRING, "glowtarget" ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Specify which variables to send to the client
//-----------------------------------------------------------------------------
IMPLEMENT_SERVERCLASS_ST( CBaseHoloPanel, DT_BaseHoloPanel )

	SendPropEHandle( SENDINFO( _animation ) ),

END_SEND_TABLE()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CBaseHoloPanel::Spawn()
{
	BaseClass::Spawn();

	// This entity can only be triggered once every second.
	m_flWait = 1.0f;

	InitEntityGlow();
	InitAnimation();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CBaseHoloPanel::UpdateOnRemove()
{
	RemoveEntityGlow();
	RemoveAnimation();
	BaseClass::UpdateOnRemove();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int CBaseHoloPanel::UpdateTransmitState()
{
	// Always transmit this entity to all clients.
	return SetTransmitState( FL_EDICT_ALWAYS );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CBaseHoloPanel::PassesTriggerFilters( CBaseEntity *pOther )
{
	if( !pOther )
	{
		return false;
	}

	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
	Assert( pPlayer );

	CBaseEntity *pHand = pPlayer->GetHandEntity();
	if( !pHand )
	{
		return false;
	}

	// Only allow the hand to interact with Holodeck triggers.
	return pHand->entindex() == pOther->entindex();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CBaseHoloPanel::InitEntityGlow()
{
	_glowTarget.Set( NULL );
	if( _glowTargetName != NULL_STRING )
	{
		// Find the entity that we must make glow.
		_glowTarget = gEntList.FindEntityByName( NULL, STRING( _glowTargetName ) );

		if( !_glowTarget )
		{
			// The entity name doesn't exist.
			Warning( "HOLODECK: Gesture panel references invalid glow entity \"%s\"\n", STRING( _glowTargetName ) );
		}
		else
		{
			// Ensure the target entity is a model based entity.
			CBaseAnimating *target = dynamic_cast<CBaseAnimating *>( _glowTarget.Get() );
			if( !target )
			{
				Warning( "HOLODECK: Gesture panel references non studio model glow entity \"%s\"\n", STRING( _glowTargetName ) );
			}
			else
			{
				target->AddGlowEffect();
			}
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CBaseHoloPanel::RemoveEntityGlow()
{
	CBaseAnimating *target = dynamic_cast<CBaseAnimating *>( _glowTarget.Get() );
	if( target )
	{
		target->RemoveGlowEffect();
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CBaseHoloPanel::InitAnimation()
{
	_animation.Set( NULL );
	if( UsesAnimatedSprite() )
	{
		CBaseEntity *ent = CreateEntityByName( "env_sprite_oriented" );
		if( ent )
		{
			CSpriteOriented *sprite = dynamic_cast<CSpriteOriented *>( ent );
			if( sprite )
			{
				// Setup per panel properties.
				sprite->SpriteInit( GetAnimatedSpritePath(), GetAbsOrigin() );
				sprite->SetScale( GetAnimatedSpriteScale() );
				sprite->SetAbsAngles( GetAnimatedSpriteAngles() );
				sprite->m_nRenderMode = kRenderWorldGlow;

				// Setup generic engine properties.
				sprite->SetSolid( SOLID_NONE );
				sprite->SetMoveType( MOVETYPE_NONE );
				UTIL_SetSize( sprite, vec3_origin, vec3_origin );

				sprite->TurnOn();

				_animation.Set( sprite );
			}
			else
			{
				ent->Remove();
				Warning( "CBaseHoloPanel::InitAnimation - Created entity was not of type CSpriteOriented!\n" );
			}
		}
		else
		{
			Warning( "CBaseHoloPanel::InitAnimation - Failed to create animated sprite!\n" );
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CBaseHoloPanel::RemoveAnimation()
{
	if( _animation.Get() )
	{
		_animation->Remove();
		_animation.Set( NULL );
	}
}