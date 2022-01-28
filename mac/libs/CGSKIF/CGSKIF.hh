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

// Mac OS
#ifndef CGIMAGE_H_
#ifndef __CGIMAGE__
#include <CGImage.h>
#endif
#endif

// rasterlib
#include "raster/load.hh"


CGImageRef CGSKIFCreateImageFromRaster( const raster::raster_load& raster );

#endif
