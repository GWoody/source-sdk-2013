/*
===============================================================================

	grid_proptool.cpp
	Implements the estate prop spawning tool.

===============================================================================
*/

#include "cbase.h"
#include "grid_proptool.h"
#include "holodeck/holo_player.h"
#include "holodeck/holo_hand.h"

LINK_ENTITY_TO_CLASS( grid_proptool, CGridPropTool );

class CTraceIgnorePlayerAndHands : public ITraceFilter
{
public:
	CTraceIgnorePlayerAndHands( CHoloPlayer *player )
	{
		_player = player;
	}
	bool ShouldHitEntity( IHandleEntity *pServerEntity, int contentsMask )
	{
		return false;
	}
	virtual TraceType_t	GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

private:
	CHoloPlayer *_player;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CGridPropTool::CGridPropTool() : BaseClass( "scripts/grid_proptool.txt" )
{
	_handIdx = -1;
	_preview.Set( NULL );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridPropTool::TakeOut( EHand h )
{
	BaseClass::TakeOut( h );
	AddEffects( EF_NODRAW );

	//
	// Create the preview prop.
	//
	CHoloPlayer *player = (CHoloPlayer *)GetOwnerEntity();
	CHoloHand *hand = player->GetHandEntity( h );
	const CFinger &finger = hand->GetFrame().GetHand( h ).GetFingerByType( FINGER_POINTER );

	CDynamicProp *preview = dynamic_cast< CDynamicProp * >( CreateEntityByName( "prop_dynamic_override" ) );
	Assert( preview );
	{
		trace_t tr;
		UTIL_TraceLine( finger.GetTipPosition(), finger.GetTipPosition() + finger.GetDirection() * 1024, 0, hand, COLLISION_GROUP_NONE, &tr );

		char buf[512];
		// Pass in standard key values
		Q_snprintf( buf, sizeof(buf), "%.10f %.10f %.10f", tr.endpos.x, tr.endpos.y, tr.endpos.z);
		preview->KeyValue( "origin", buf );
		Q_snprintf( buf, sizeof(buf), "%.10f %.10f %.10f", 0, 0, 0 );
		preview->KeyValue( "angles", buf );
		preview->KeyValue( "model", _prop.Get() );
		preview->KeyValue( "solid", "0" );
		preview->KeyValue( "fademindist", "-1" );
		preview->KeyValue( "fademaxdist", "0" );
		preview->KeyValue( "fadescale", "1" );
		preview->KeyValue( "MinAnimTime", "5" );
		preview->KeyValue( "MaxAnimTime", "10" );
		preview->Precache();
		DispatchSpawn( preview );
		preview->Activate();
		preview->SetRenderColorA( 63 );

		_handIdx = h;
		_preview.Set( preview );
	}

	hand->SetInvisible( false );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridPropTool::PutAway()
{
	BaseClass::PutAway();

	//
	// Destroy the preview prop.
	//
	_preview->Remove();
	_preview.Set( NULL );

	_handIdx = -1;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CGridPropTool::ItemPreFrame()
{
	if( _handIdx >= 0 )
	{
		CHoloPlayer *player = (CHoloPlayer *)GetOwnerEntity();
		CHoloHand *hand = player->GetHandEntity( (EHand)_handIdx );
		const CFinger &finger = hand->GetFrame().GetHand( (EHand)_handIdx ).GetFingerByType( FINGER_POINTER );

		CTraceFilterWorldAndPropsOnly filter;
		trace_t tr;
		//UTIL_TraceLine( finger.GetTipPosition(), finger.GetTipPosition() + finger.GetDirection() * 1024, 0, hand, COLLISION_GROUP_NONE, &tr );
		UTIL_TraceLine( finger.GetTipPosition(), finger.GetTipPosition() + finger.GetDirection() * 1024, MASK_SOLID, &filter, &tr );
		//UTIL_TraceModel( finger.GetTipPosition(), finger.GetTipPosition() + finger.GetDirection() * 1024, _preview->CollisionProp()->OBBMins(), _preview->CollisionProp()->OBBMaxs(), player, COLLISION_GROUP_NONE, &tr );
		Msg( "%f %f %f\n", tr.endpos.x, tr.endpos.y, tr.endpos.z );

		debugoverlay->AddLineOverlay( finger.GetTipPosition(), tr.endpos, 255, 255, 255, false, 0.5 );
		
		_preview->SetAbsOrigin( tr.endpos );
	}

	BaseClass::ItemPreFrame();
}