/*	=================
 *	TrackScrollbar.hh
 *	=================
 */

#ifndef PEDESTAL_TRACKSCROLLBAR_HH
#define PEDESTAL_TRACKSCROLLBAR_HH

// Nitrogen
#include "Nitrogen/Controls.h"


namespace Pedestal
{
	
	class ScrollerAPI;
	
	typedef void ( *ControlTracker )( ControlRef                 control,
	                                  Nitrogen::ControlPartCode  part,
	                                  Point                      point );
	
	typedef ScrollerAPI* ( *ScrollerGetter )( ControlRef control );
	
	typedef void ( *TrackDebriefer )( ControlRef control );
	
	struct Scrollbar_UserData
	{
		ControlTracker  trackHook;
		ScrollerGetter  fetchHook;
		TrackDebriefer  afterHook;
		bool            vertical;
		
		Scrollbar_UserData() : trackHook(), fetchHook(), afterHook(), vertical()
		{
		}

	};
	
	void TrackScrollbar( ControlRef control, Nitrogen::ControlPartCode part, Point point );
	
}

#endif

