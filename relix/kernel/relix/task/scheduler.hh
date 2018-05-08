/*
	scheduler.hh
	------------
*/

#ifndef RELIX_TASK_SCHEDULER_HH
#define RELIX_TASK_SCHEDULER_HH

// POSIX
#include <sys/types.h>


namespace relix
{
	
	struct queue_element;
	struct task;
	
	void acquire_sync_semaphore();
	void release_sync_semaphore();
	
	void pthread_wakeup();
	void pthread_yield();
	
#ifdef __RELIX__
	
	inline
	void acquire_sync_semaphore() {}
	
	inline
	void release_sync_semaphore() {}
	
	inline
	void pthread_yield() {}
	
#endif
	
	void async_resume_task( task* that );
	
	void resume_pending_tasks();
	
	queue_element* schedule_next_task();
	
	void create_task( task* that );
	void remove_task( task* that );
	
	void suspend_task( task* that );
	void resume_task ( task* that );
	
	bool is_active();
	
	bool is_on_run_queue( const task* that );
	
	
	class synchronized_scope
	{
		private:
			// non-copyable
			synchronized_scope           ( const synchronized_scope& );
			synchronized_scope& operator=( const synchronized_scope& );
		
		public:
			synchronized_scope()
			{
				acquire_sync_semaphore();
			}
			
			~synchronized_scope()
			{
				release_sync_semaphore();
			}
	};
	
	class unsynchronized_scope
	{
		private:
			// non-copyable
			unsynchronized_scope           ( const unsynchronized_scope& );
			unsynchronized_scope& operator=( const unsynchronized_scope& );
		
		public:
			unsynchronized_scope()
			{
				release_sync_semaphore();
			}
			
			~unsynchronized_scope()
			{
				acquire_sync_semaphore();
			}
	};
	
}

#endif
