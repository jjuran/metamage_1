/*
	desktop.cc
	----------
*/

#include "desktop.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __LOWMEM__
#include <LowMem.h>
#endif
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif


void set_desktop_pattern( const Pattern& pattern )
{
#if ! TARGET_API_MAC_CARBON
	
	LMSetDeskPattern( &pattern );
	
	PaintOne( NULL, LMGetWMgrPort()->visRgn );
	
#endif
}
