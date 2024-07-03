/*
	fullscreen.cc
	-------------
*/

#include "fullscreen.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// QDFullscreen
#include "fullscreen_QT.hh"
#include "fullscreen_window.hh"


static inline
bool has_QT_v2_5()
{
	if ( TARGET_API_MAC_CARBON )
	{
		/*
			Actually, we might be running in Mac OS 9 with the QuickTime
			extension disabled, but in this scenario, Carbon applications
			that use QuickTime don't launch at all, even if you weak-link
			CarbonLib -- because CarbonLib strong-links InterfaceLib and
			can't resolve GetComponentResource().  So if a Carbon app
			gets here in the first place, we can assume we have QuickTime.
		*/
		return true;
	}
	
	const SInt32 v2_5 = 0x02500000;
	
	return mac::sys::gestalt( 'qtim' ) >= v2_5;
}

static const bool has_QT_fullscreen = has_QT_v2_5();

namespace fullscreen {

bool in_effect;

void enter()
{
	if ( has_QT_fullscreen )
	{
		begin_fullscreen();
	}
	else if ( ! TARGET_API_MAC_CARBON )
	{
		open_fullscreen_window();
	}
	
	in_effect = true;
}

void leave()
{
	if ( has_QT_fullscreen )
	{
		end_fullscreen();
	}
	else if ( ! TARGET_API_MAC_CARBON )
	{
		close_fullscreen_window();
	}
	
	in_effect = false;
	
	SetPortWindowPort( FrontWindow() );
}

}
