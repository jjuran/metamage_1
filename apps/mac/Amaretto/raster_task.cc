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
using raster::raster_metadata;
using raster::sync_relay;

static bool monitoring;

static poseven::thread raster_thread;

raster_event_set raster_events;

static CFRunLoopRef        mainRunLoop;
static CFRunLoopSourceRef  inputSource;

static
void signal_runloop()
{
	CFRunLoopSourceSignal( inputSource );
	
	CFRunLoopWakeUp( mainRunLoop );
}

static
void raster_event_loop( const clut_data* clut, const sync_relay* sync )
{
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
			
			raster_events.cursorBits = true;
		}
		
		if ( clut  &&  clut->seed != colors_seed )
		{
			colors_seed = clut->seed;
			
			raster_events.newPalette = true;
		}
		
		if ( raster_seed != sync->seed )
		{
			raster_seed = sync->seed;
			
			raster_events.screenBits = true;
		}
		
		raster_events.repaintDue = true;
		
		signal_runloop();
	}
	
	raster_events.rasterDone = true;
	
	signal_runloop();
}

static
void* raster_thread_entry( void* arg )
{
	const raster_metadata* meta = (const raster_metadata*) arg;
	
	const clut_data*  clut = find_clut( &meta->note );
	const sync_relay* sync = find_sync( &meta->note );
	
	raster_event_loop( clut, sync );
	
	return NULL;
}

raster_monitor::raster_monitor( const raster::raster_load&  load,
                                perform_proc                perform )
{
	CFRunLoopSourceContext context =
	{
		0,
		&raster_events,
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
	
	raster_thread.create( &raster_thread_entry, (void*) load.meta );
}

raster_monitor::~raster_monitor()
{
	monitoring = false;
	
	raster_thread.join();
	
	CFRunLoopRemoveSource( mainRunLoop, inputSource, kCFRunLoopCommonModes );
	
	CFRelease( inputSource );
}
