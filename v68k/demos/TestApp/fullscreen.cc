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

// TestApp
#include "fullscreen_QT.hh"
#include "fullscreen_port.hh"


void enter_fullscreen()
{
	if ( TARGET_API_MAC_CARBON )
	{
		begin_fullscreen();
	}
	else
	{
		clobber_screen();
	}
}

void leave_fullscreen()
{
	if ( TARGET_API_MAC_CARBON )
	{
		end_fullscreen();
	}
	else
	{
		refresh_screen();
	}
	
	SetPortWindowPort( FrontWindow() );
}
