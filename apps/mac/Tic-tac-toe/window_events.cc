/*
	window_events.cc
	----------------
*/

#include "window_events.hh"

// Mac OS
#ifndef __CARBONEVENTS__
#include <CarbonEvents.h>
#endif

// mac-config
#include "mac_config/upp-macros.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"

// Tic-tac-toe
#include "window.hh"


using mac::qd::get_portRect;


static
pascal OSStatus window_WindowBoundsChange( EventHandlerCallRef  handler,
                                           EventRef             event,
                                           void*                userData )
{
	OSStatus err;
	
	WindowRef window;
	UInt32 attributes;
	(err = GetEventParameter( event,
	                          kEventParamDirectObject,
	                          typeWindowRef,
	                          NULL,
	                          sizeof window,
	                          NULL,
	                          &window ))  ||
	(err = GetEventParameter( event,
	                          kEventParamAttributes,
	                          typeUInt32,
	                          NULL,
	                          sizeof attributes,
	                          NULL,
	                          &attributes ));
	
	if ( err == noErr  &&  attributes & kWindowBoundsChangeSizeChanged )
	{
		const Rect& portRect = get_portRect( window );
		
		window_size_changed( portRect );
		draw_window        ( portRect );
	}
	
	return err;
}

DEFINE_CARBON_UPP( EventHandler, window_WindowBoundsChange )

static EventTypeSpec windowBoundsChange_event[] =
{
	{ kEventClassWindow, kEventWindowBoundsChanging },
	{ kEventClassWindow, kEventWindowBoundsChanged  },
};

void install_window_event_handlers( WindowRef window )
{
	OSStatus err;
	
	if ( TARGET_API_MAC_CARBON  &&  mac::sys::gestalt( 'sysv' ) >= 0x1000 )
	{
		/*
			Even without kWindowLiveResizeAttribute set in the attributes,
			this event will repeatedly fire in Mac OS 9 while the window is
			being resized, to hideous, graphically glitchy effect.
		*/
		
		err = InstallWindowEventHandler( window,
		                                 UPP_ARG( window_WindowBoundsChange ),
		                                 2,
		                                 windowBoundsChange_event,
		                                 NULL,
		                                 NULL );
	}
}