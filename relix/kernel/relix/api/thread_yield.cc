/*
	thread_yield.cc
	---------------
*/

#include "relix/api/thread_yield.hh"

// relix
#include "relix/api/current_thread.hh"
#include "relix/api/os_thread_api.hh"
#include "relix/signal/unstoppable.hh"
#include "relix/task/process.hh"
#include "relix/task/thread.hh"


namespace relix
{
	
	static inline
	bool stopped( const thread& pthread )
	{
		const process& proc = pthread.get_process();
		
		if ( proc.is_stopped() )
		{
			const sigset_t pending     = pthread.signals_pending();
			const sigset_t unstoppable = unstoppable_signals( pthread );
			
		#ifndef __linux__
			
			return !( pending & unstoppable );
			
		#endif
		}
		
		return false;
	}
	
	void thread_yield()
	{
		thread& current = current_thread();
		
		do
		{
			current.mark_current_stack_frame();
			
			os_thread_yield();
		}
		while ( stopped( current ) );
	}
	
}
