//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#ifndef SUN_H
#define SUN_H

class CSun : public CBaseEntity
{
public:
	DECLARE_CLASS( CSun, CBaseEntity );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CSun();

	virtual void	Activate();

	// Input handlers
	void InputTurnOn( inputdata_t &inputdata );
	void InputTurnOff( inputdata_t &inputdata );
	void InputSetColor( inputdata_t &inputdata );

	virtual int UpdateTransmitState();

public:
	CNetworkVector( m_vDirection );
	
	string_t	m_strMaterial;
	string_t	m_strOverlayMaterial;

	int		m_bUseAngles;
	float	m_flPitch;
	float	m_flYaw;
	
	CNetworkVar( int, m_nSize );		// Size of the main core image
	CNetworkVar( int, m_nOverlaySize ); // Size for the glow overlay
	CNetworkVar( color32, m_clrOverlay );
	CNetworkVar( bool, m_bOn );
	CNetworkVar( int, m_nMaterial );
	CNetworkVar( int, m_nOverlayMaterial );
	CNetworkVar( float, m_flHDRColorScale );
};

#endif