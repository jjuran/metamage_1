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
	const void*            addr;
	const unsigned short*  clut_palette;
	unsigned               clut_maximum;
	
	bool screenBits;
	bool cursorBits;
	bool newPalette;
	bool repaintDue;
	bool rasterDone;
};

class display_monitor
{
	NON_COPYABLE( display_monitor )
	
	public:
		typedef void (*perform_proc)( void* info );
		
		display_monitor( const raster::raster_load&  load,
		                 perform_proc                perform );
		
		~display_monitor();
};

extern display_event_set display_events;

#endif
