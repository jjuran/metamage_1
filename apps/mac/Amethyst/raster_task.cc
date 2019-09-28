/*
	raster_task.cc
	--------------
*/

#include "raster_task.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// POSIX
#include <unistd.h>

// config
#include "config/setpshared.h"

// raster
#include "raster/raster.hh"
#include "raster/relay.hh"
#include "raster/relay_detail.hh"
#include "raster/sync.hh"

// poseven
#include "poseven/types/thread.hh"


extern raster::raster_load loaded_raster;

static poseven::thread raster_thread;

static
raster::sync_relay* find_sync()
{
	using namespace raster;
	
	raster_note* sync = find_note( *loaded_raster.meta, Note_sync );
	
	return (sync_relay*) data( sync );
}

static
void raster_event_loop( raster::sync_relay* sync )
{
	const OSType   eventClass = kEventClassAmethyst;
	const uint32_t eventID    = kEventAmethystUpdate;
	
	EventQueueRef queue = GetMainEventQueue();
	
	EventRef update;
	CreateEvent( NULL, eventClass, eventID, 0, kEventAttributeNone, &update );
	
	uint32_t seed = 0;
	
	bool wait_is_broken = ! CONFIG_SETPSHARED;
	
	while ( sync->status == raster::Sync_ready )
	{
		while ( seed == sync->seed )
		{
			if ( wait_is_broken )
			{
				usleep( 10000 );  // 10ms
			}
			else
			{
				try
				{
					raster::wait( *sync );
				}
				catch ( const raster::wait_failed& )
				{
					//ERROR( POLLING_ENSUES );
					
					wait_is_broken = true;
				}
			}
			
			poseven::thread::testcancel();
		}
		
		seed = sync->seed;
		
		PostEventToQueue( queue, update, kEventPriorityStandard );
	}
	
	ReleaseEvent( update );
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

raster_monitor::raster_monitor()
{
	GetMainEventQueue();  // initialization is thread-unsafe before 10.4
	
	raster::sync_relay* sync = find_sync();
	
	publish( *sync );
	
	raster_thread.create( &raster_thread_entry, sync );
}

raster_monitor::~raster_monitor()
{
	raster_thread.join();
}
