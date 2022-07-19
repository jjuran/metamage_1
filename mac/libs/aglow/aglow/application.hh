/*
	application.hh
	--------------
*/

#ifndef AGLOW_APPLICATION_HH
#define AGLOW_APPLICATION_HH


namespace raster
{
	
	struct raster_load;
	struct raster_desc;
	
}

namespace amicus
{

void initialize();

void run_event_loop( const raster::raster_load&  load,
                     const raster::raster_desc&  desc );

}

#endif
