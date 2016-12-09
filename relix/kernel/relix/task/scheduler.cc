/*
	scheduler.cc
	------------
*/

#include "relix/task/scheduler.hh"

// Standard C++
#include <set>

// must
#include "must/pthread.h"


namespace relix
{
	
	static std::set< pid_t > active_threads;
	
	static bool pthread_yield_needed;
	
	static pthread_mutex_t sync_mutex = PTHREAD_MUTEX_INITIALIZER;
	
	
	/*
		This locks the sync mutex on behalf of the main thread before any
		other thread has a chance.
	*/
	static synchronized_scope main_scope;
	
	
	void pthread_wakeup()
	{
		pthread_yield_needed = true;
	}
	
#ifndef __RELIX__
	
	void acquire_sync_semaphore()
	{
		must_pthread_mutex_lock( &sync_mutex );
	}
	
	void release_sync_semaphore()
	{
		must_pthread_mutex_unlock( &sync_mutex );
	}
	
	void pthread_yield()
	{
		unsynchronized_scope unsync;
		
		sched_yield();
	}
	
#endif
	
	
	bool is_active()
	{
		if ( pthread_yield_needed )
		{
			pthread_yield_needed = false;
			
			pthread_yield();
		}
		
		return !active_threads.empty();
	}
	
	void mark_thread_active( pid_t tid )
	{
		active_threads.insert( tid );
	}
	
	void mark_thread_inactive( pid_t tid )
	{
		active_threads.erase( tid );
	}
	
}
