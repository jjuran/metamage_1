/*
	load.hh
	-------
*/

#ifndef RASTER_LOAD_HH
#define RASTER_LOAD_HH

// raster
#include "raster/raster.hh"


namespace raster
{
	
	raster_load load_raster( int fd );
	
	raster_load create_raster( int fd );
	
	void unload_raster( raster_load& loaded );
	
}

#endif
