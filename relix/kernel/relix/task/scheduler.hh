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
	
	bool is_active();
	
	bool is_thread_active( pid_t tid );
	
	void mark_thread_active( pid_t tid );
	
	void mark_thread_inactive( pid_t tid );
	
	
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
