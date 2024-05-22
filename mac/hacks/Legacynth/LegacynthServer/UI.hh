/*
	UI.hh
	-----
*/

#ifndef UI_HH
#define UI_HH

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


struct ControlRecord;

typedef ControlRecord** ControlRef;


enum
{
	Apple = 1,
	File  = 2,
	
	About = 1,
	
	Start   = 1,
	Stop    = 2,
	// ---- = 3,
	Quit    = 4,
};

extern WindowRef console_window;

extern ControlRef start_stop_button;

void startup();
void cleanup();

#endif
