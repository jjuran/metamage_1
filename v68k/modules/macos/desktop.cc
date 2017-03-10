/*
	desktop.cc
	----------
*/

#include "desktop.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


GrafPtr WMgrPort    : 0x09DE;
Pattern DeskPattern : 0x0A3C;


void draw_desktop_from_WMgrPort()
{
	// WMgrPort and a suitable clipRgn have already been set.
	
	FillRect( &WMgrPort->portRect, &DeskPattern );
}
