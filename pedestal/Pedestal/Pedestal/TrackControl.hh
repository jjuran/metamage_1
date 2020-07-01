/*	===============
 *	TrackControl.hh
 *	===============
 */

#ifndef PEDESTAL_TRACKCONTROL_HH
#define PEDESTAL_TRACKCONTROL_HH

// Nitrogen
#ifndef MAC_CONTROLS_TYPES_CONTROLPARTCODE_HH
#include "Mac/Controls/Types/ControlPartCode.hh"
#endif


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
	
	
	void TrackControl( ControlRef control, ControlPartCode part, Point point );
	
}

#endif
