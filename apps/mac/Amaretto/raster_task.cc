/*
	raster_task.cc
	--------------
*/

#include "raster_task.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

// v68k-cursor
#include "cursor/cursor.hh"

// rasterlib
#include "raster/clut.hh"
#include "raster/clut_detail.hh"
#include "raster/raster.hh"
#include "raster/relay_detail.hh"
#include "raster/sync.hh"

// poseven
#include "poseven/types/thread.hh"

// frontend-common
#include "frend/cursor.hh"
#include "frend/update_fifo.hh"


using raster::clut_data;
using raster::raster_load;
using raster::raster_metadata;
using raster::sync_relay;

static bool monitoring;

static poseven::thread display_thread;

display_event_set display_events;

static CFRunLoopRef        mainRunLoop;
static CFRunLoopSourceRef  inputSource;

static
void signal_runloop()
{
	CFRunLoopSourceSignal( inputSource );
	
	CFRunLoopWakeUp( mainRunLoop );
}

static
void* display_thread_body( void* arg )
{
	const raster_load&     load = *(const raster_load*) arg;
	const raster_metadata& meta = *load.meta;
	
	const clut_data*  clut = find_clut( &meta.note );
	const sync_relay* sync = find_sync( &meta.note );
	
	if ( clut )
	{
		display_events.clut_palette = &clut->palette[ 0 ].value;
		display_events.clut_maximum = clut->max;
	}
	
	uint32_t raster_seed = 0;
	uint32_t colors_seed = 0;
	uint16_t cursor_seed = 0;
	
	while ( monitoring  &&  sync->status == raster::Sync_ready )
	{
		using frend::cursor_state;
		using frend::wait_for_update;
		
		wait_for_update();
		
		if ( cursor_state  &&  cursor_state->seed != cursor_seed )
		{
			cursor_seed = cursor_state->seed;
			
			display_events.cursorBits = true;
		}
		
		if ( clut  &&  clut->seed != colors_seed )
		{
			colors_seed = clut->seed;
			
			display_events.newPalette = true;
		}
		
		if ( raster_seed != sync->seed )
		{
			raster_seed = sync->seed;
			
			display_events.screenBits = true;
		}
		
		display_events.repaintDue = true;
		
		signal_runloop();
	}
	
	display_events.rasterDone = true;
	
	signal_runloop();
	
	return NULL;
}

display_monitor::display_monitor( const raster::raster_load&  load,
                                  perform_proc                perform )
{
	CFRunLoopSourceContext context =
	{
		0,
		&display_events,
		NULL,  // retain
		NULL,  // release
		NULL,  // copyDescription
		NULL,  // equal
		NULL,  // hash
		NULL,  // schedule
		NULL,  // cancel
		perform,
	};
	
	mainRunLoop = CFRunLoopGetCurrent();
	inputSource = CFRunLoopSourceCreate( NULL, 0, &context );
	
	CFRunLoopAddSource( mainRunLoop, inputSource, kCFRunLoopCommonModes );
	
	monitoring = true;
	
	display_thread.create( &display_thread_body, (void*) &load );
}

display_monitor::~display_monitor()
{
	monitoring = false;
	
	display_thread.join();
	
	CFRunLoopRemoveSource( mainRunLoop, inputSource, kCFRunLoopCommonModes );
	
	CFRelease( inputSource );
}
