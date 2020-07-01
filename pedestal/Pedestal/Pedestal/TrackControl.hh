/*	===============
 *	TrackControl.hh
 *	===============
 */

#ifndef PEDESTAL_TRACKCONTROL_HH
#define PEDESTAL_TRACKCONTROL_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __CONTROLS__
#include <Controls.h>
#endif


namespace Pedestal
{
	
	typedef void ( *ControlTracker )( ControlRef       control,
	                                  ControlPartCode  part,
	                                  Point            point );
	
	typedef void ( *TrackDebriefer )( ControlRef control );
	
	struct Control_UserData
	{
		ControlTracker  trackHook;
		TrackDebriefer  afterHook;
		
		Control_UserData() : trackHook(), afterHook()
		{
		}
	};
	
	
	void TrackControl( ControlRef control, ControlPartCode part, Point point );
	
}

#endif
