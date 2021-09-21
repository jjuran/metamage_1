/*
	raster_task.cc
	--------------
*/

#include "amicus/raster_task.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// rasterlib
#include "raster/raster.hh"
#include "raster/relay.hh"
#include "raster/relay_detail.hh"
#include "raster/sync.hh"

// poseven
#include "poseven/types/thread.hh"


namespace amicus
{

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
	const OSType   eventClass = kEventClassAmicus;
	const uint32_t eventID    = kEventAmicusUpdate;
	
	EventQueueRef queue = GetMainEventQueue();
	
	EventRef update;
	CreateEvent( NULL, eventClass, eventID, 0, kEventAttributeNone, &update );
	
	uint32_t seed = 0;
	
	sigset_t mask;
	sigprocmask( SIG_SETMASK, NULL, &mask );  // get blocked signal mask
	sigdelset  ( &mask, SIGUSR1 );
	
	while ( sync->status == raster::Sync_ready )
	{
		while ( seed == sync->seed )
		{
			sigsuspend( &mask );
			
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

static
void sigusr1( int sig )
{
}

raster_monitor::raster_monitor()
{
	// Block SIGUSR1 and install an empty signal handler
	
	sigset_t mask;
	sigemptyset( &mask );
	sigaddset  ( &mask, SIGUSR1 );
	sigprocmask( SIG_BLOCK, &mask, NULL );
	
	signal( SIGUSR1, &sigusr1 );
	
	GetMainEventQueue();  // initialization is thread-unsafe before 10.4
	
	raster::sync_relay* sync = find_sync();
	
	publish( *sync );
	
	raster_thread.create( &raster_thread_entry, sync );
}

raster_monitor::~raster_monitor()
{
	raster_thread.join();
}

}
