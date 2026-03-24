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

struct display_event_set
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

extern display_event_set display_events;

#endif
