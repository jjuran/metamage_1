/*	=================
 *	TrackScrollbar.hh
 *	=================
 */

#ifndef PEDESTAL_TRACKSCROLLBAR_HH
#define PEDESTAL_TRACKSCROLLBAR_HH

// Pedestal
#include "Pedestal/TrackControl.hh"


namespace Pedestal
{
	
	class ScrollerAPI;
	
	typedef ScrollerAPI* ( *ScrollerGetter )( ControlRef control );
	
	struct Scrollbar_UserData : Control_UserData
	{
		ScrollerGetter  fetchHook;
		bool            vertical;
		
		Scrollbar_UserData() : fetchHook(), vertical()
		{
		}
	};
	
	void TrackScrollbar( ControlRef control, Mac::ControlPartCode part, Point point );
	
}

#endif

