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
		explicit raster_monitor( const raster::raster_load& load );
		
		~raster_monitor();
};

extern raster_event_set raster_events;

extern CFRunLoopRef        mainRunLoop;
extern CFRunLoopSourceRef  inputSource;

#endif
