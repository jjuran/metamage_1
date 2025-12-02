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


Fixed maximum_zoom( short window_width, short window_height );

WindowRef create_window( unsigned width, unsigned height, WindowRef previous );

#endif
