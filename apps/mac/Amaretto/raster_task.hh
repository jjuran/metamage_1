/*
	raster_task.hh
	--------------
*/

#ifndef RASTERTASK_HH
#define RASTERTASK_HH

// iota
#include "iota/class.hh"


namespace raster
{
	
	struct raster_load;
	
}

struct raster_event_set
{
	bool screenBits;
	bool cursorBits;
	bool newPalette;
	bool repaintDue;
	bool rasterDone;
};

class raster_monitor
{
	NON_COPYABLE( raster_monitor )
	
	public:
		typedef void (*perform_proc)( void* info );
		
		raster_monitor( const raster::raster_load&  load,
		                perform_proc                perform );
		
		~raster_monitor();
};

extern raster_event_set raster_events;

#endif
