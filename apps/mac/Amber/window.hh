/*
	window.hh
	---------
*/

#ifndef WINDOW_HH
#define WINDOW_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif


namespace raster
{
	struct raster_desc;
}

extern short x_numer;
extern short x_denom;

Fixed maximum_zoom( short window_width, short window_height );

WindowRef create_window( const raster::raster_desc& desc, WindowRef previous );

#endif
