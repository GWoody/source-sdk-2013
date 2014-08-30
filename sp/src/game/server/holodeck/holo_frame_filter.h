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

		holo::CFrame	FilterFrame( const holo::CFrame &frame );

	private:
		void			AddToHistory( const holo::CFrame &frame );

		// Filtering methods.
		holo::CFrame	StandardAverage();
		holo::CFrame	AgedAverage();

		CUtlVector<holo::CFrame> _history;
	};
	
}

#endif // __HOLO_FRAME_FILTER_H__