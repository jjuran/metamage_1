/*
	window.hh
	---------
*/

#ifndef WINDOWIMAGE_HH
#define WINDOWIMAGE_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


WindowRef create_window();

void draw_window( WindowRef window );

#endif
