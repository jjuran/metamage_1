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


namespace raster
{
	struct raster_load;
}

using raster::raster_load;

WindowRef create_window( const raster_load& load, ConstStr255Param    name );
WindowRef create_window( const raster_load& load, const HFSUniStr255& name );

void destroy_window( WindowRef window );

void draw_window( WindowRef window );

#endif
