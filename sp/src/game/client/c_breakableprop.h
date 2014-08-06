//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef C_BREAKABLEPROP_H
#define C_BREAKABLEPROP_H
#ifdef _WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class C_BreakableProp : public C_BaseAnimating
{
	typedef C_BaseAnimating BaseClass;
public:
	DECLARE_CLIENTCLASS();

	C_BreakableProp();
	
	virtual void SetFadeMinMax( float fademin, float fademax );

	// Copy fade from another breakable prop
	void CopyFadeFrom( C_BreakableProp *pSource );

#ifdef HOLODECK
public:
	virtual void	Simulate();
	virtual void	GetGlowEffectColor( float *r, float *g, float *b );

private:
	float			_glowAlpha;
#endif
};

#endif // C_BREAKABLEPROP_H
