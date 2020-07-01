/*	=================
 *	TrackScrollbar.hh
 *	=================
 */

#ifndef PEDESTAL_TRACKSCROLLBAR_HH
#define PEDESTAL_TRACKSCROLLBAR_HH

// Pedestal
#ifndef PEDESTAL_TRACKCONTROL_HH
#include "Pedestal/TrackControl.hh"
#endif


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
	
	void TrackScrollbar( ControlRef control, ControlPartCode part, Point point );
	
}

#endif
