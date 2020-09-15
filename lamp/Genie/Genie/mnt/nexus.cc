/*
	nexus.cc
	--------
*/

// relix-kernel
#include "relix/api/os_thread_api.hh"
#include "relix/api/os_thread_box.hh"
#include "relix/task/scheduler.hh"


#ifndef NULL
#define NULL  0L
#endif


namespace relix
{
	
	/*
		The nexus thread joins the preemptive pthreads world with the
		cooperative Mac application world.  It acts as an interlock --
		it holds the sync mutex on behalf of the cooperative application
		threads while in os_thread_yield() (which calls YieldToAnyThread()),
		preventing other pthreads from using it.  Once os_thread_yield()
		returns, it calls pthread_yield(), which relinquishes the sync mutex
		to other pthreads and reacquires it before returning and calling
		os_thread_yield() again (until which no cooperative threads can run).
	*/
	
	static
	void* nexus_start( void* param, const void* bottom, const void* limit )
	{
		for ( ;; )
		{
			pthread_yield();
			os_thread_yield();
		}
	}
	
	
#ifndef __RELIX__
	
	static os_thread_box nexus_thread = new_os_thread( &nexus_start, NULL, 0 );
	
#endif
	
}
