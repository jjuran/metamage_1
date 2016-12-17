/*
	relay.cc
	--------
*/

#include "raster/relay.hh"

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
		
	#ifndef __RELIX__
		
		const int pshared = PTHREAD_PROCESS_SHARED;
		
		must_pthread_mutexattr_init( &mutex_attr );
		must_pthread_condattr_init ( &cond_attr  );
		
		must_pthread_mutexattr_setpshared( &mutex_attr, pshared );
		must_pthread_condattr_setpshared ( &cond_attr,  pshared );
		
	#endif
		
		must_pthread_mutex_init( &relay.mutex, &mutex_attr );
		must_pthread_cond_init ( &relay.cond,  &cond_attr  );
		
	#ifndef __RELIX__
		
		must_pthread_mutexattr_destroy( &mutex_attr );
		must_pthread_condattr_destroy ( &cond_attr  );
		
	#endif
		
		relay.reserved = 0;
		relay.seed     = 0;
	}
	
	void unpublish( sync_relay& relay )
	{
		must_pthread_mutex_destroy( &relay.mutex );
		must_pthread_cond_destroy ( &relay.cond  );
	}
	
	void broadcast( sync_relay& relay )
	{
		++relay.seed;
		
		must_pthread_cond_broadcast( &relay.cond );
	}
	
	void wait( sync_relay& relay )
	{
		must_pthread_mutex_lock  (              &relay.mutex );
		must_pthread_cond_wait   ( &relay.cond, &relay.mutex );
		must_pthread_mutex_unlock(              &relay.mutex );
	}
	
}
