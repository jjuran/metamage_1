/*
	window.hh
	---------
*/

#ifndef AGLOW_WINDOW_HH
#define AGLOW_WINDOW_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif


extern const short desk_height;
extern const short desk_width;

WindowRef create_window( unsigned width, unsigned height, WindowRef previous );

#endif
