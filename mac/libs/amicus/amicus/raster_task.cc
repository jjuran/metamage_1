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
#include <sys/stat.h>
#include <unistd.h>

// rasterlib
#include "raster/raster.hh"
#include "raster/relay.hh"
#include "raster/relay_detail.hh"
#include "raster/sync.hh"

// poseven
#include "poseven/types/thread.hh"


namespace amicus
{

static const char update_fifo[] = "update-signal.fifo";

static bool monitoring;

static poseven::thread raster_thread;

static
void sigchld( int )
{
	open( update_fifo, O_RDONLY | O_NONBLOCK );
}

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
	const uint32_t eventID    = kEventAmicusUpdate;
	
	EventQueueRef queue = GetMainEventQueue();
	
	EventRef update;
	CreateEvent( NULL, eventClass, eventID, 0, kEventAttributeNone, &update );
	
	uint32_t seed = 0;
	
	while ( monitoring  &&  sync->status == raster::Sync_ready )
	{
		while ( monitoring  &&  seed == sync->seed )
		{
			close( open( update_fifo, O_WRONLY ) );
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

raster_monitor::raster_monitor( const raster::raster_load& load )
{
	signal( SIGCHLD, &sigchld );
	
	GetMainEventQueue();  // initialization is thread-unsafe before 10.4
	
	mkfifo( update_fifo, 0666 );
	
	raster::sync_relay* sync = find_sync( load );
	
	publish( *sync );
	
	monitoring = true;
	
	raster_thread.create( &raster_thread_entry, sync );
}

raster_monitor::~raster_monitor()
{
	monitoring = false;
	
	raster_thread.join();
	
	unlink( update_fifo );
}

}
