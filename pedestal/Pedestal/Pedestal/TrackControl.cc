/*
	TrackControl.cc
	---------------
*/

#include "Pedestal/TrackControl.hh"

// Debug
#include "debug/assert.hh"


namespace Pedestal
{
	
	void TrackControl( ControlRef control, ControlPartCode part, Point point )
	{
		// Did we actually hit a control?
		ASSERT( part != kControlNoPart );
		
		// If the control has an action routine, it's not a Pedestal control.
		ASSERT( GetControlAction( control ) == NULL );
		
		Control_UserData* userData = (Control_UserData*) GetControlReference( control );
		
		ASSERT( userData != NULL );
		
		part = TrackControl( control, point, NULL );
		
		if ( part != kControlNoPart )
		{
			if ( TrackDebriefer afterHook = userData->afterHook )
			{
				afterHook( control );
			}
		}
	}
	
}
