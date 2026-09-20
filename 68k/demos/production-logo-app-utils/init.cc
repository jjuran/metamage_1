/*
	init.cc
	-------
*/

#include "init.hh"

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


void init()
{
	InitGraf( &qd.thePort );
	
	InitFonts();
	InitWindows();
	InitMenus();
	
	HideCursor();  // in case the cursor is shown automatically
}
