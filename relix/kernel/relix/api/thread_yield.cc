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
#include "relix/task/scheduler.hh"
#include "relix/task/thread.hh"


namespace relix
{
	
	static inline
	bool stopped( const thread& pthread )
	{
		const process& proc = pthread.get_process();
		
		if ( proc.is_stopped() )
		{
		#ifndef __linux__
			
			const sigset_t pending = pthread.signals_pending()
			                       | proc.signals_pending();
			
		#endif
			
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
			
			suspend_task( current.get_task() );
			
			os_thread_yield();
			
			resume_task( current.get_task() );
		}
		while ( stopped( current ) );
	}
	
	void thread_yield_active()
	{
		thread& current = current_thread();
		
		const pid_t tid = current.id();
		
		do
		{
			current.mark_current_stack_frame();
			
			os_thread_yield();
		}
		while ( stopped( current ) );
	}
	
}
