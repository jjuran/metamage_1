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
	
	void pthread_yield()
	{
		must_pthread_mutex_unlock( &sync_mutex );
		
		sched_yield();
		
		must_pthread_mutex_lock( &sync_mutex );
	}
	
#endif
	
	
	bool is_active()
	{
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
	
	
#ifndef __RELIX__
	
	synchronized_scope::synchronized_scope()
	{
		must_pthread_mutex_lock( &sync_mutex );
	}
	
	synchronized_scope::~synchronized_scope()
	{
		must_pthread_mutex_unlock( &sync_mutex );
	}
	
#endif
	
}
