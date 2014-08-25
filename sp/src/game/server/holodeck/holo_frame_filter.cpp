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

	COUNT
};

//-----------------------------------------------------------------------------
// ConVars
//-----------------------------------------------------------------------------
static const char *holo_filter_method_help_string = "Filtering methods:\n\
0 - average of `holo_filter_history` frames,\n\
";
static ConVar holo_filter_method( "holo_filter_method", "0", FCVAR_ARCHIVE, holo_filter_method_help_string );

static ConVar holo_filter_history( "holo_filter_history", "10", FCVAR_ARCHIVE, "Number of frames to use in the filtering operation." );

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

	CFrame f = _history[0];
	for( int i = 1; i < _history.Count(); i++ )
	{
		f = f + _history[i];
	}

	f = f / _history.Count();
	return f;
}