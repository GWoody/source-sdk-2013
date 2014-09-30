//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Sunlight shadow control entity.
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "tier0/vprof.h"
#include "c_sunlightshadowcontrol.h"

#include "c_baseplayer.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


ConVar cl_sunlight_ortho_size("cl_sunlight_ortho_size", "0.0", FCVAR_CHEAT, "Set to values greater than 0 for ortho view render projections.");
ConVar cl_sunlight_slopescaledepthbias( "cl_sunlight_slopescaledepthbias", "2", FCVAR_CHEAT );
ConVar cl_sunlight_depthbias( "cl_sunlight_depthbias", "0.0" );

ClientShadowHandle_t C_SunlightShadowControl::m_LocalFlashlightHandle = CLIENTSHADOW_INVALID_HANDLE;


IMPLEMENT_CLIENTCLASS_DT(C_SunlightShadowControl, DT_SunlightShadowControl, CSunlightShadowControl)
	RecvPropVector(RECVINFO(m_shadowDirection)),
	RecvPropBool(RECVINFO(m_bEnabled)),
	RecvPropString(RECVINFO(m_TextureName)),
	RecvPropArray( RecvPropFloat(RECVINFO(m_LightColor[0]), 0), m_LightColor ),
	RecvPropFloat(RECVINFO(m_flColorTransitionTime)),
	RecvPropFloat(RECVINFO(m_flSunDistance)),
	RecvPropFloat(RECVINFO(m_flFOV)),
	RecvPropFloat(RECVINFO(m_flNearZ)),
	RecvPropFloat(RECVINFO(m_flNorthOffset)),
	RecvPropBool(RECVINFO(m_bEnableShadows)),
END_RECV_TABLE()


C_SunlightShadowControl::~C_SunlightShadowControl()
{
	if ( m_LocalFlashlightHandle != CLIENTSHADOW_INVALID_HANDLE )
	{
		g_pClientShadowMgr->DestroyFlashlight( m_LocalFlashlightHandle );
		m_LocalFlashlightHandle = CLIENTSHADOW_INVALID_HANDLE;
	}
}

void C_SunlightShadowControl::OnDataChanged( DataUpdateType_t updateType )
{
	if ( updateType == DATA_UPDATE_CREATED )
	{
		m_SpotlightTexture.Init( m_TextureName, TEXTURE_GROUP_OTHER, true );
	}

	BaseClass::OnDataChanged( updateType );
}

void C_SunlightShadowControl::Spawn()
{
	BaseClass::Spawn();

	m_bOldEnableShadows = m_bEnableShadows;

	SetNextClientThink( CLIENT_THINK_ALWAYS );
}

//------------------------------------------------------------------------------
// We don't draw...
//------------------------------------------------------------------------------
bool C_SunlightShadowControl::ShouldDraw()
{
	return false;
}

void C_SunlightShadowControl::ClientThink()
{
	VPROF("C_SunlightShadowControl::ClientThink");
	if ( m_bEnabled )
	{
		Vector vLinearFloatLightColor( m_LightColor[0], m_LightColor[1], m_LightColor[2] );
		float flLinearFloatLightAlpha = m_LightColor[3];

		if ( m_CurrentLinearFloatLightColor != vLinearFloatLightColor || m_flCurrentLinearFloatLightAlpha != flLinearFloatLightAlpha )
		{
			float flColorTransitionSpeed = gpGlobals->frametime * m_flColorTransitionTime * 255.0f;

			m_CurrentLinearFloatLightColor.x = Approach( vLinearFloatLightColor.x, m_CurrentLinearFloatLightColor.x, flColorTransitionSpeed );
			m_CurrentLinearFloatLightColor.y = Approach( vLinearFloatLightColor.y, m_CurrentLinearFloatLightColor.y, flColorTransitionSpeed );
			m_CurrentLinearFloatLightColor.z = Approach( vLinearFloatLightColor.z, m_CurrentLinearFloatLightColor.z, flColorTransitionSpeed );
			m_flCurrentLinearFloatLightAlpha = Approach( flLinearFloatLightAlpha, m_flCurrentLinearFloatLightAlpha, flColorTransitionSpeed );
		}

		FlashlightState_t state;

		Vector vDirection = m_shadowDirection;
		VectorNormalize( vDirection );

		QAngle angView;
		engine->GetViewAngles( angView );

		//Vector vViewUp = Vector( 0.0f, 1.0f, 0.0f );
		Vector vSunDirection2D = vDirection;
		vSunDirection2D.z = 0.0f;

		if ( !C_BasePlayer::GetLocalPlayer() )
			return;

		Vector vPos = ( C_BasePlayer::GetLocalPlayer()->GetAbsOrigin() + vSunDirection2D * m_flNorthOffset ) - vDirection * m_flSunDistance;

		QAngle angAngles;
		VectorAngles( vDirection, angAngles );

		Vector vForward, vRight, vUp;
		AngleVectors( angAngles, &vForward, &vRight, &vUp );

		state.m_fHorizontalFOVDegrees = m_flFOV;
		state.m_fVerticalFOVDegrees = m_flFOV;

		state.m_vecLightOrigin = vPos;
		BasisToQuaternion( vForward, vRight, vUp, state.m_quatOrientation );

		state.m_fQuadraticAtten = 0.0f;
		state.m_fLinearAtten = m_flSunDistance / 2.0f;
		state.m_fConstantAtten = 0.0f;
		state.m_Color[0] = m_CurrentLinearFloatLightColor.x * ( 1.0f / 255.0f ) * m_flCurrentLinearFloatLightAlpha;
		state.m_Color[1] = m_CurrentLinearFloatLightColor.y * ( 1.0f / 255.0f ) * m_flCurrentLinearFloatLightAlpha;
		state.m_Color[2] = m_CurrentLinearFloatLightColor.z * ( 1.0f / 255.0f ) * m_flCurrentLinearFloatLightAlpha;
		state.m_Color[3] = 0.0f; // fixme: need to make ambient work m_flAmbient;
		state.m_NearZ = fpmax( 4.0f, m_flSunDistance - m_flNearZ );
		state.m_FarZ = m_flSunDistance + 300.0f;

		AswFlashlightState_t aswState;
		aswState.m_bShadowHighRes = true;

		float flOrthoSize = cl_sunlight_ortho_size.GetFloat();
		if ( flOrthoSize > 0 )
		{
			aswState.m_bOrtho = true;
			aswState.m_fOrthoLeft = -flOrthoSize;
			aswState.m_fOrthoTop = -flOrthoSize;
			aswState.m_fOrthoRight = flOrthoSize;
			aswState.m_fOrthoBottom = flOrthoSize;
		}
		else
		{
			aswState.m_bOrtho = false;
		}

		state.m_flShadowSlopeScaleDepthBias = cl_sunlight_slopescaledepthbias.GetFloat();
		state.m_flShadowDepthBias = cl_sunlight_depthbias.GetFloat();
		state.m_bEnableShadows = m_bEnableShadows;
		state.m_pSpotlightTexture = m_SpotlightTexture;
		state.m_nSpotlightTextureFrame = 0;

		state.m_nShadowQuality = 0; // Allow entity to affect shadow quality

		if ( m_bOldEnableShadows != m_bEnableShadows )
		{
			// If they change the shadow enable/disable, we need to make a new handle
			if ( m_LocalFlashlightHandle != CLIENTSHADOW_INVALID_HANDLE )
			{
				g_pClientShadowMgr->DestroyFlashlight( m_LocalFlashlightHandle );
				m_LocalFlashlightHandle = CLIENTSHADOW_INVALID_HANDLE;
			}

			m_bOldEnableShadows = m_bEnableShadows;
		}

		if( m_LocalFlashlightHandle == CLIENTSHADOW_INVALID_HANDLE )
		{
			m_LocalFlashlightHandle = g_pClientShadowMgr->CreateFlashlight( state, &aswState );
		}
		else
		{
			g_pClientShadowMgr->UpdateFlashlightState( m_LocalFlashlightHandle, state, &aswState );
#ifndef INFESTED_DLL
#pragma message("TODO: rebuild sunlight projected texture after sunlight control changes.")
			g_pClientShadowMgr->UpdateProjectedTexture( m_LocalFlashlightHandle, true );
#endif
		}
	}
	else if ( m_LocalFlashlightHandle != CLIENTSHADOW_INVALID_HANDLE )
	{
		g_pClientShadowMgr->DestroyFlashlight( m_LocalFlashlightHandle );
		m_LocalFlashlightHandle = CLIENTSHADOW_INVALID_HANDLE;
	}

	BaseClass::ClientThink();
}
