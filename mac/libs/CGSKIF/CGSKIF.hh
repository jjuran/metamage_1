/*
	CGSKIF.hh
	---------
*/

#ifndef CGSKIF_HH
#define CGSKIF_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// rasterlib
#include "raster/load.hh"


CGImageRef CGSKIFCreateImageFromRaster( const raster::raster_load& raster );

#endif
