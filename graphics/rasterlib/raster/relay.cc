/*
	relay.cc
	--------
*/

#include "raster/relay.hh"

// Standard C
#include <errno.h>

// config
#include "config/setpshared.h"

// must
#include "must/pthread.h"

// raster
#include "raster/relay_detail.hh"


namespace raster
{
	
	void publish( sync_relay& relay )
	{
		pthread_mutexattr_t mutex_attr;
		pthread_condattr_t  cond_attr;
		
		/*
			We initialize and destroy the mutex and condvar attributes
			even if PTHREAD_PROCESS_SHARED isn't supported, because it
			makes the source code simpler, and the scenario in which it's
			wasted work is already negatively affected by the lack of
			support.  If, on the other hand, setting the attribute were
			the workaround (rather than not setting it), then I'd consider
			extra compile-time logic to avoid adding a run-time burden to
			the normal case.
		*/
		
		must_pthread_mutexattr_init( &mutex_attr );
		must_pthread_condattr_init ( &cond_attr  );
		
	#if CONFIG_SETPSHARED
		
		const int pshared = PTHREAD_PROCESS_SHARED;
		
		must_pthread_mutexattr_setpshared( &mutex_attr, pshared );
		must_pthread_condattr_setpshared ( &cond_attr,  pshared );
		
	#endif
		
		must_pthread_mutex_init( &relay.mutex, &mutex_attr );
		must_pthread_cond_init ( &relay.cond,  &cond_attr  );
		
		must_pthread_mutexattr_destroy( &mutex_attr );
		must_pthread_condattr_destroy ( &cond_attr  );
		
		relay.status = Sync_ready;
		relay.seed   = 0;
	}
	
	void unpublish( sync_relay& relay )
	{
		must_pthread_mutex_destroy( &relay.mutex );
		must_pthread_cond_destroy ( &relay.cond  );
	}
	
	void broadcast( sync_relay& relay )
	{
		++relay.seed;
		
	#if !defined(__MSYS__) && !defined(__CYGWIN__)
		
		must_pthread_cond_broadcast( &relay.cond );
		
	#endif
	}
	
	void terminate( sync_relay& relay )
	{
		relay.status = Sync_ended;
		
		broadcast( relay );
	}
	
	void wait( sync_relay& relay )
	{
	#if CONFIG_SETPSHARED
		
		must_pthread_mutex_lock( &relay.mutex );
		
		int err = pthread_cond_wait( &relay.cond, &relay.mutex );
		
		must_pthread_mutex_unlock( &relay.mutex );
		
	#else
		
		const int err = ENOSYS;
		
	#endif
		
		if ( err )
		{
			wait_failed exception = { err };
			
			throw exception;
		}
	}
	
}
