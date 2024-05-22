/*
	window.hh
	---------
*/

#ifndef WINDOW_HH
#define WINDOW_HH

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


void create_window( OSErr err );

void draw_window( WindowRef window );

void content_click( WindowRef window, Point where );

#endif
