//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Sunlight shadow control entity.
//
// $NoKeywords: $
//=============================================================================//

#ifndef SUNLIGHTSHADOWCONTROL_H
#define SUNLIGHTSHADOWCONTROL_H

//------------------------------------------------------------------------------
// Purpose : Sunlight shadow control entity
//------------------------------------------------------------------------------
class CSunlightShadowControl : public CBaseEntity
{
public:
	DECLARE_CLASS( CSunlightShadowControl, CBaseEntity );

	CSunlightShadowControl();

	void Spawn( void );
	bool KeyValue( const char *szKeyName, const char *szValue );
	virtual bool GetKeyValue( const char *szKeyName, char *szValue, int iMaxLen );
	int  UpdateTransmitState();

	// Inputs
	void	InputSetAngles( inputdata_t &inputdata );
	void	InputEnable( inputdata_t &inputdata );
	void	InputDisable( inputdata_t &inputdata );
	void	InputSetTexture( inputdata_t &inputdata );
	void	InputSetEnableShadows( inputdata_t &inputdata );
	void	InputSetLightColor( inputdata_t &inputdata );

	virtual int	ObjectCaps( void ) { return BaseClass::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

protected:
	CNetworkVector( m_shadowDirection );

	CNetworkVar( bool, m_bEnabled );
	bool m_bStartDisabled;

	CNetworkString( m_TextureName, MAX_PATH );
	CNetworkColor32( m_LightColor );
	CNetworkVar( float, m_flColorTransitionTime );
	CNetworkVar( float, m_flSunDistance );
	CNetworkVar( float, m_flFOV );
	CNetworkVar( float, m_flNearZ );
	CNetworkVar( float, m_flNorthOffset );
	CNetworkVar( bool, m_bEnableShadows );
};

#endif // SUNLIGHTSHADOWCONTROL_H