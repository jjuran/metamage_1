/*
	screen_spec.hh
	--------------
*/

#ifndef FREND_SCREENSPEC_HH
#define FREND_SCREENSPEC_HH

// rasterlib
#include "raster/spec.hh"


namespace frend
{

using raster::raster_spec;

extern raster_spec screen_spec;

long load_screen_spec( int bindir_fd );

}

#endif
