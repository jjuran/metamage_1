/*	===============
 *	PedScrollbar.cc
 *	===============
 */

#include "Pedestal/Scrollbar.hh"

// Universal Interfaces
#include <ControlDefinitions.h>

// Nitrogen
#include "Nitrogen/ControlDefinitions.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	Scrollbar::Scrollbar( const Rect&       bounds,
	                      N::ControlProcID  procID,
	                      N::RefCon         refCon,
	                      ControlTracker    tracker )
	: control( N::NewControl( N::GetWindowFromPort( N::GetQDGlobalsThePort() ),
	                          bounds,
	                          "\p",
	                          true,
	                          0,
	                          0,
	                          0,
	                          procID,
	                          &controlHooks ) )
	{
		controlHooks.data      = refCon;
		controlHooks.trackHook = tracker;
	}
	
	void Scrollbar::Activate( bool activating )
	{
		if ( activating )
		{
			//N::ShowControl( control );
			N::HiliteControl( control, kControlNoPart );
		}
		else
		{
			//N::HideControl( control );
			N::HiliteControl( control, kControlInactivePart );
		}
	}
	
}

