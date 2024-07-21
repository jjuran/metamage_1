/*
	window.hh
	---------
*/

#ifndef CHESS_WINDOW_HH
#define CHESS_WINDOW_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


extern WindowRef main_window;

short make_main_window();

#endif
