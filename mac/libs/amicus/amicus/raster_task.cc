/*
	raster_task.cc
	--------------
*/

#include "amicus/raster_task.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// POSIX
#include <fcntl.h>
#include <unistd.h>

// v68k-cursor
#include "cursor/cursor.hh"

// rasterlib
#include "raster/raster.hh"
#include "raster/relay_detail.hh"
#include "raster/sync.hh"

// poseven
#include "poseven/types/thread.hh"

// amicus
#include "amicus/cursor.hh"


namespace amicus
{

static bool monitoring;

static poseven::thread raster_thread;

static
raster::sync_relay* find_sync( const raster::raster_load& load )
{
	using namespace raster;
	
	raster_note* sync = find_note( *load.meta, Note_sync );
	
	return (sync_relay*) data( sync );
}

static
void raster_event_loop( raster::sync_relay* sync )
{
	const OSType   eventClass = kEventClassAmicus;
	const uint32_t repaintDue = kEventAmicusUpdate;
	const uint32_t screenBits = kEventAmicusScreenBits;
	const uint32_t cursorBits = kEventAmicusCursorBits;
	
	EventQueueRef queue = GetMainEventQueue();
	
	EventRef repaint_due;
	EventRef screen_bits;
	EventRef cursor_bits;
	CreateEvent( NULL, eventClass, repaintDue, 0, kEventAttributeNone, &repaint_due );
	CreateEvent( NULL, eventClass, screenBits, 0, kEventAttributeNone, &screen_bits );
	CreateEvent( NULL, eventClass, cursorBits, 0, kEventAttributeNone, &cursor_bits );
	
	uint32_t raster_seed = 0;
	uint16_t cursor_seed = 0;
	
	while ( monitoring  &&  sync->status == raster::Sync_ready )
	{
		close( open( UPDATE_FIFO, O_WRONLY ) );
		
		if ( cursor_state  &&  cursor_state->seed != cursor_seed )
		{
			cursor_seed = cursor_state->seed;
			
			PostEventToQueue( queue, cursor_bits, kEventPriorityHigh );
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
}

static
void* raster_thread_entry( void* arg )
{
	OSStatus err;
	
	raster::sync_relay* sync = (raster::sync_relay*) arg;
	
	raster_event_loop( sync );
	
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
	
	raster::sync_relay* sync = find_sync( load );
	
	sync->status = raster::Sync_ready;
	sync->seed   = 0;
	
	monitoring = true;
	
	raster_thread.create( &raster_thread_entry, sync );
}

raster_monitor::~raster_monitor()
{
	monitoring = false;
	
	raster_thread.join();
}

}
