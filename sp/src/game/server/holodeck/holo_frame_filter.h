/*
===============================================================================

	holo_frame_filter.h
	Defines the interface to the Holodeck frame filtering system.

===============================================================================
*/

#ifndef __HOLO_FRAME_FILTER_H__
#define __HOLO_FRAME_FILTER_H__

#include "holodeck/holo_shared.h"

namespace holo
{

	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	class CFrameFilter
	{
	public:
						CFrameFilter();

		CFrame			FilterFrame( const CFrame &frame );
		const CFrame &	GetLastFrame() const		{ return _lastFrame; }

	private:
		void			AddToHistory( const CFrame &frame );

		// Filtering methods.
		CFrame	StandardAverage();
		CFrame	AgedAverage();

		CUtlVector<CFrame> _history;
		CFrame			_lastFrame;
	};
	
}

#endif // __HOLO_FRAME_FILTER_H__