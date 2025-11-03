/*
	raster_task.cc
	--------------
*/

#include "amicus/raster_task.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
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


namespace amicus
{

using raster::clut_data;
using raster::raster_metadata;
using raster::sync_relay;

static bool monitoring;

static poseven::thread raster_thread;

static
void raster_event_loop( const clut_data* clut, const sync_relay* sync )
{
	const OSType   eventClass = kEventClassAmicus;
	const uint32_t repaintDue = kEventAmicusUpdate;
	const uint32_t screenBits = kEventAmicusScreenBits;
	const uint32_t cursorBits = kEventAmicusCursorBits;
	const uint32_t newPalette = kEventAmicusNewPalette;
	
	EventQueueRef queue = GetMainEventQueue();
	
	EventRef repaint_due;
	EventRef screen_bits;
	EventRef cursor_bits;
	EventRef new_palette;
	CreateEvent( NULL, eventClass, repaintDue, 0, kEventAttributeNone, &repaint_due );
	CreateEvent( NULL, eventClass, screenBits, 0, kEventAttributeNone, &screen_bits );
	CreateEvent( NULL, eventClass, cursorBits, 0, kEventAttributeNone, &cursor_bits );
	CreateEvent( NULL, eventClass, newPalette, 0, kEventAttributeNone, &new_palette );
	
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
			
			PostEventToQueue( queue, cursor_bits, kEventPriorityHigh );
		}
		
		if ( clut  &&  clut->seed != colors_seed )
		{
			colors_seed = clut->seed;
			
			PostEventToQueue( queue, new_palette, kEventPriorityHigh );
		}
		
		if ( raster_seed != sync->seed )
		{
			raster_seed = sync->seed;
			
			PostEventToQueue( queue, screen_bits, kEventPriorityHigh );
		}
		
		PostEventToQueue( queue, repaint_due, kEventPriorityStandard );
	}
	
	ReleaseEvent( repaint_due );
	ReleaseEvent( screen_bits );
	ReleaseEvent( cursor_bits );
	ReleaseEvent( new_palette );
}

static
void* raster_thread_entry( void* arg )
{
	OSStatus err;
	
	const raster_metadata* meta = (const raster_metadata*) arg;
	
	const clut_data*  clut = find_clut( &meta->note );
	const sync_relay* sync = find_sync( &meta->note );
	
	raster_event_loop( clut, sync );
	
	EventRef quitEvent;
	err = CreateEvent( NULL,
	                   kEventClassApplication,
	                   kEventAppQuit,
	                   0,
	                   kEventAttributeNone,
	                   &quitEvent );
	
	EventQueueRef queue = GetMainEventQueue();
	
	err = PostEventToQueue( queue, quitEvent, kEventPriorityHigh );
	
	return NULL;
}

raster_monitor::raster_monitor( const raster::raster_load& load )
{
	GetMainEventQueue();  // initialization is thread-unsafe before 10.4
	
	monitoring = true;
	
	raster_thread.create( &raster_thread_entry, (void*) load.meta );
}

raster_monitor::~raster_monitor()
{
	monitoring = false;
	
	raster_thread.join();
}

}
