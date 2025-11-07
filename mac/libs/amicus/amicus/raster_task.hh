/*
	raster_task.hh
	--------------
*/

#ifndef AMICUS_RASTERTASK_HH
#define AMICUS_RASTERTASK_HH

// iota
#include "iota/class.hh"


namespace raster
{
	
	struct raster_load;
	
}

namespace amicus
{

enum
{
	kEventClassAmicus = 'AMS*',
	
	kEventAmicusUpdate = 1,
	kEventAmicusScreenBits = 2,
	kEventAmicusCursorBits = 3,
	kEventAmicusNewPalette = 4,
};

class raster_monitor
{
	NON_COPYABLE( raster_monitor )
	
	public:
		explicit raster_monitor( const raster::raster_load& load );
		
		~raster_monitor();
};

}

#endif
