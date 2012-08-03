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
	
	void pthread_wakeup();
	void pthread_yield();
	
#ifdef __RELIX__
	
	inline
	void pthread_yield() {}
	
#endif
	
	bool is_active();
	
	void mark_thread_active( pid_t tid );
	
	void mark_thread_inactive( pid_t tid );
	
	
	class synchronized_scope
	{
	#ifndef __RELIX__
		
		private:
			// non-copyable
			synchronized_scope           ( const synchronized_scope& );
			synchronized_scope& operator=( const synchronized_scope& );
		
		public:
			synchronized_scope();
			
			~synchronized_scope();
		
	#endif
	};
	
}

#endif
