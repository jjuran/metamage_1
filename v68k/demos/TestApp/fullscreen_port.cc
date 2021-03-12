/*
	fullscreen_port.cc
	------------------
*/

#include "fullscreen_port.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __LOWMEM__
#include <LowMem.h>
#endif
#ifndef __MENUS__
#include <Menus.h>
#endif

// mac-sys-utils
#include "mac_sys/gestalt.hh"


#if ! TARGET_API_MAC_CARBON

static GrafPort fullscreen_port;

#endif

void clobber_screen()
{
#if ! TARGET_API_MAC_CARBON
	
	if ( fullscreen_port.visRgn == NULL )
	{
		OpenPort( &fullscreen_port );
	}
	else
	{
		SetPort( &fullscreen_port );
	}
	
	FillRect( &fullscreen_port.portRect, &qd.black );
	
#endif
}

void refresh_screen()
{
	DrawMenuBar();
	
#if ! TARGET_API_MAC_CARBON
	
	PaintBehind( LMGetWindowList(), LMGetGrayRgn() );
	
	/*
		This is just to make sure that fullscreen_port is no longer set as
		the current port.  We call SetPortWindowPort( FrontWindow() ) below,
		but we can't rely on it setting thePort if there are no windows.
		(In Mac OS 9 at least, it ignores NULL arguments.)
	*/
	
	qd.thePort = LMGetWMgrPort();
	
#endif
}

void cleanup_screen()
{
#if ! TARGET_API_MAC_CARBON
	
	if ( qd.thePort == &fullscreen_port )
	{
		if ( ! TARGET_CPU_68K  ||  ! mac::sys::gestalt_defined( 'v68k' ) )
		{
			refresh_screen();
		}
	}
	
#endif
}
