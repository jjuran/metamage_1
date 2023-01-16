/*
	relay.cc
	--------
*/

#include "raster/relay.hh"

// config
#include "config/setpshared.h"

// must
#include "must/pthread.h"

// raster
#include "raster/relay_detail.hh"


/*
	The mutex and condition variable are currently only needed in a
	process-shared configuration.  If this changes, you can always define
	CONFIG_CONDVARS true with CONFIG_SETPSHARED false.
*/

#ifndef CONFIG_CONDVARS
#define CONFIG_CONDVARS  CONFIG_SETPSHARED
#endif

#if ! CONFIG_CONDVARS
#undef CONFIG_SETPSHARED
#define CONFIG_SETPSHARED  0
#endif


namespace raster
{
	
	void publish( sync_relay& relay )
	{
	#if CONFIG_CONDVARS
		
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
		
	#endif  // #if CONFIG_CONDVARS
		
		relay.status = Sync_ready;
		relay.seed   = 0;
	}
	
	void unpublish( sync_relay& relay )
	{
	#if CONFIG_CONDVARS
		
		must_pthread_mutex_destroy( &relay.mutex );
		must_pthread_cond_destroy ( &relay.cond  );
		
	#endif
	}
	
	void broadcast( sync_relay& relay )
	{
		++relay.seed;
		
	#if CONFIG_CONDVARS
		
		must_pthread_cond_broadcast( &relay.cond );
		
	#endif
	}
	
	void terminate( sync_relay& relay )
	{
		relay.status = Sync_ended;
		
		++relay.seed;
		
	#if CONFIG_CONDVARS
		
		/*
			Allow errors -- xv68k in Amber gets EINVAL here in Mac OS X 10.7.
			
			I have no idea why this happens.  We never destroy the condvar
			or unmap the memory segment.  We don't even unlink the raster
			file until after xv68k exits (which shouldn't matter anyway).
		*/
		
		int err = pthread_cond_broadcast( &relay.cond );
		
	#endif
	}
	
}
