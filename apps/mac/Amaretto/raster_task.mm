/*
	raster_task.mm
	--------------
*/

#include "raster_task.hh"

// Mac OS X
#include <Cocoa/Cocoa.h>

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

id raster_event_target = nil;

static
void post( SEL selector )
{
	[raster_event_target performSelectorOnMainThread: selector
	                     withObject:                  nil
	                     waitUntilDone:               NO];
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
			
			post( @selector(onCursorBits) );
		}
		
		if ( clut  &&  clut->seed != colors_seed )
		{
			colors_seed = clut->seed;
			
			post( @selector(onNewPalette) );
		}
		
		if ( raster_seed != sync->seed )
		{
			raster_seed = sync->seed;
			
			post( @selector(onScreenBits) );
		}
		
		post( @selector(onRepaintDue) );
	}
	
	post( @selector(onRasterDone) );
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

raster_monitor::raster_monitor( const raster::raster_load& load )
{
	monitoring = true;
	
	raster_thread.create( &raster_thread_entry, (void*) load.meta );
}

raster_monitor::~raster_monitor()
{
	monitoring = false;
	
	raster_thread.join();
}
