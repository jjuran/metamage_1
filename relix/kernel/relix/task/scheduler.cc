/*
	scheduler.cc
	------------
*/

#include "relix/task/scheduler.hh"

// must
#include "must/pthread.h"

// poseven
#include "poseven/types/thread.hh"

// relix-kernel
#include "relix/task/queue.hh"
#include "relix/task/task.hh"
#include "relix/task/thread.hh"


namespace relix
{
	
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
		poseven::thread::testcancel();
		
		unsynchronized_scope unsync;
		
		sched_yield();
	}
	
#endif
	
	
	static queue run_queue;
	static queue wait_queue;
	
	static bool resumptions_pending;
	
	void async_resume_task( task* that )
	{
		that->pending = true;
		
		resumptions_pending = true;
	}
	
	void resume_pending_tasks()
	{
		resumptions_pending = false;
		
		queue_element* it = wait_queue.get();
		
		while ( it != NULL )
		{
			task* that = static_cast< task* >( it );
			
			it = it->next;
			
			if ( that->pending )
			{
				wait_queue.remove( that );
				run_queue.prepend( that );
				
				that->pending = false;
			}
		}
	}
	
	queue_element* schedule_next_task()
	{
		if ( resumptions_pending )
		{
			resume_pending_tasks();
		}
		
		queue_element* next = run_queue.get();
		
		if ( next )
		{
			run_queue.remove( next );
			run_queue.append( next );
		}
		
		return next;
	}
	
	void create_task( task* that )
	{
		run_queue.prepend( that );
	}
	
	void remove_task( task* that )
	{
		if ( that->from )
		{
			that->from->remove( that );
			
			that->from = NULL;
		}
	}
	
	void suspend_task( task* that )
	{
		if ( that->from == &run_queue )
		{
			run_queue .remove ( that );
			wait_queue.prepend( that );
		}
	}
	
	void resume_task( task* that )
	{
		if ( that->from == &wait_queue )
		{
			wait_queue.remove( that );
			run_queue.prepend( that );
			
			that->pending = false;
		}
	}
	
	bool is_active()
	{
		if ( pthread_yield_needed )
		{
			pthread_yield_needed = false;
			
			pthread_yield();
		}
		
		if ( resumptions_pending )
		{
			resume_pending_tasks();
		}
		
		return ! run_queue.empty();
	}
	
	bool is_on_run_queue( const task* that )
	{
		return that  &&  that->from == &run_queue;
	}
	
}
