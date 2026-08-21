/*
	invalidate_window.hh
	--------------------
*/

#include "mac_app/invalidate_window.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"
#include "mac_qd/globals/thePort.hh"
#include "mac_qd/is_port_visrgn_empty.hh"

// mac-ui-utils
#include "mac_ui/windows.hh"


namespace mac {
namespace app {

using mac::ui::invalidate_if_compositing;

#if ! __LP64__

void invalidate_window( WindowRef window )
{
#ifdef MAC_OS_X_VERSION_10_2
	
	if ( invalidate_if_compositing( window ) )
	{
		return;
	}
	
#endif
	
	CGrafPtr port = GetWindowPort( window );
	
	if ( ! mac::qd::is_port_visrgn_empty( port ) )
	{
		const Rect& portRect = mac::qd::get_portRect( port );
		
	#if ! OPAQUE_TOOLBOX_STRUCTS
		
		GrafPtr thePort = mac::qd::thePort();
		
		if ( thePort != window )
		{
			SetPort( window );
		}
		
		InvalRect( &portRect );
		
		if ( thePort != window )
		{
			SetPort( thePort );
		}
		
		return;
		
	#endif
		
		InvalWindowRect( window, &portRect );
	}
}

#else

int dummy;

#endif  // #if ! __LP64__

}
}
