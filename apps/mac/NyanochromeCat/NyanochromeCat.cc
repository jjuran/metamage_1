/*
	NyanochromeCat.cc
	-----------------
*/

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// mac-config
#include "mac_config/apple-events.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// mac-qd-utils
#include "mac_qd/set_cursor_by_id.hh"

// mac-app-utils
#include "mac_app/event_handlers.hh"
#include "mac_app/init.hh"
#include "mac_app/menus.hh"

// NyanochromeCat
#include "About.hh"
#include "CarbonEventLoop.hh"
#include "ClassicEventLoop.hh"
#include "Offscreen.hh"
#include "Timer.hh"
#include "Window.hh"


#pragma exceptions off


// gestaltAppleEventsAttr = 'evnt'

const bool apple_events_present =
	CONFIG_APPLE_EVENTS  &&
		(CONFIG_APPLE_EVENTS_GRANTED  ||
			mac::sys::gestalt( 'evnt' ) != 0);

int main()
{
	mac::app::init_toolbox();
	mac::app::install_menus();
	
	if ( ! TARGET_API_MAC_CARBON  &&  apple_events_present )
	{
		mac::app::install_basic_event_handlers();
	}
	
	OSErr err = mac::qd::set_cursor_by_id( watchCursor );
	
	render_offscreen();
	
	if ( err == noErr )
	{
		InitCursor();
	}
	
	make_main_window();
	
	draw_window( main_window );
	
	animation_timer.start();
	
	if ( TARGET_API_MAC_CARBON )
	{
		Carbon_event_loop();
	}
	else
	{
		classic_event_loop();
	}
	
	return 0;
}
