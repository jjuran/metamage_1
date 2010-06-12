/*	===============
 *	TrackControl.hh
 *	===============
 */

#ifndef PEDESTAL_TRACKCONTROL_HH
#define PEDESTAL_TRACKCONTROL_HH

// Nitrogen
#include "Mac/Controls/Types/ControlPartCode.hh"


namespace Pedestal
{
	
	typedef void ( *ControlTracker )( ControlRef            control,
	                                  Mac::ControlPartCode  part,
	                                  Point                 point );
	
	typedef void ( *TrackDebriefer )( ControlRef control );
	
	struct Control_UserData
	{
		ControlTracker  trackHook;
		TrackDebriefer  afterHook;
		
		Control_UserData() : trackHook(), afterHook()
		{
		}
	};
	
	
	void TrackControl( ControlRef control, Mac::ControlPartCode part, Point point );
	
}

#endif

