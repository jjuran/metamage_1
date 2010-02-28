/*	===============
 *	TrackControl.cc
 *	===============
 */

#include "Pedestal/TrackControl.hh"

// Nitrogen
#include "Nitrogen/ControlDefinitions.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	void TrackControl( ControlRef control, N::ControlPartCode part, Point point )
	{
		// Did we actually hit a control?
		ASSERT( part != N::kControlNoPart );
		
		// If the control has an action routine, it's not a Pedestal control.
		ASSERT( N::GetControlAction( control ) == NULL );
		
		Control_UserData* userData = N::GetControlReference( control );
		
		ASSERT( userData != NULL );
		
		part = N::TrackControl( control, point );
		
		if ( part != N::kControlNoPart )
		{
			if ( TrackDebriefer afterHook = userData->afterHook )
			{
				afterHook( control );
			}
		}
	}
	
}

