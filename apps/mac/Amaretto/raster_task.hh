/*
	raster_task.hh
	--------------
*/

#ifndef RASTERTASK_HH
#define RASTERTASK_HH

// Mac OS X
#include <Foundation/Foundation.h>

// iota
#include "iota/class.hh"


namespace raster
{
	
	struct raster_load;
	
}

class raster_monitor
{
	NON_COPYABLE( raster_monitor )
	
	public:
		explicit raster_monitor( const raster::raster_load& load );
		
		~raster_monitor();
};

extern id raster_event_target;

#endif
