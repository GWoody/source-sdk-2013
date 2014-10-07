/*
===============================================================================

	holo_frame_filter.cpp
	Implements the Holodeck frame filtering system.

===============================================================================
*/

#include "cbase.h"
#include "holo_frame_filter.h"

using namespace holo;

enum EFilterMethod
{
	AVERAGE,
	AGED_AVERAGE,

	COUNT
};

//-----------------------------------------------------------------------------
// ConVars
//-----------------------------------------------------------------------------
static const char *holo_filter_method_help_string = "Filtering methods:\n\
0 - average of `holo_filter_history` frames,\n\
1 - average weighted towards the current frame,\n\
";
static ConVar holo_filter_method( "holo_filter_method", "0", FCVAR_ARCHIVE, holo_filter_method_help_string );

static ConVar holo_filter_history( "holo_filter_history", "10", FCVAR_ARCHIVE, "Number of frames to use in the filtering operation." );
static ConVar holo_filter_apply_confidence( "holo_filter_apply_confidence", "1", FCVAR_ARCHIVE, "Sets whether frame confidence values are used in the filter calculation." );

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CFrameFilter::CFrameFilter()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CFrame CFrameFilter::FilterFrame( const CFrame &frame )
{
	if( frame.IsValid() )
	{
		AddToHistory( frame );
	}

	switch( holo_filter_method.GetInt() )
	{
		case EFilterMethod::AVERAGE:
			return StandardAverage();

		case EFilterMethod::AGED_AVERAGE:
			return AgedAverage();
	}

	return StandardAverage();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CFrameFilter::AddToHistory( const CFrame &frame )
{
	while( _history.Count() >= holo_filter_history.GetInt() )
	{
		_history.RemoveMultipleFromHead( 1 );
	}
		
	_history.AddToTail( frame );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CFrame CFrameFilter::StandardAverage()
{
	if( !_history.Count() )
	{
		return CFrame();
	}

	CFrame sum;

	if( holo_filter_apply_confidence.GetBool() )
	{
		const float MAX_HISTORY = holo_filter_history.GetFloat();
		float totalWeight = 0.0f;

		for( int i = 0; i < _history.Count(); i++ )
		{
			// Older frames are stored at the beginning.
			float currentWeight = ( i + 1 ) / MAX_HISTORY;
			totalWeight += currentWeight;

			sum = sum + ( _history[i] * currentWeight );
		}

		sum = sum / totalWeight;
	}
	else
	{
		for( int i = 1; i < _history.Count(); i++ )
		{
			sum = sum + _history[i];
		}

		sum = sum / _history.Count();
	}	

	return sum;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CFrame CFrameFilter::AgedAverage()
{
	if( !_history.Count() )
	{
		return CFrame();
	}

	const float MAX_HISTORY = holo_filter_history.GetFloat();

	CFrame sum;
	float totalWeight = 0.0f;

	bool useconfidence = holo_filter_apply_confidence.GetBool();

	for( int i = 0; i < _history.Count(); i++ )
	{
		// Older frames are stored at the beginning.
		float currentWeight = ( i + 1 ) / MAX_HISTORY;
		float confidence = _history[i].GetHand( HAND_LEFT ).GetConfidence() + _history[i].GetHand( HAND_RIGHT ).GetConfidence();

		currentWeight *= useconfidence ? confidence : 1.0f;
		totalWeight += currentWeight;

		sum = sum + ( _history[i] * currentWeight );
	}

	sum = sum / totalWeight;
	return sum;
}