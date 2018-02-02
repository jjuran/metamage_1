/*
	breathe.cc
	----------
*/

// Standard C
#include <time.h>

// relix-kernel
#include "relix/api/breathe.hh"
#include "relix/api/current_thread.hh"
#include "relix/api/thread_yield.hh"
#include "relix/signal/check_signals.hh"
#include "relix/task/scheduler.hh"
#include "relix/task/thread.hh"


namespace relix
{
	
	static clock_t the_last_breath_time;
	
	
	bool breathe( bool may_throw )
	{
		if ( check_signals( may_throw ) )
		{
			return true;
		}
		
		const clock_t now = clock();
		
		if ( now - the_last_breath_time > 20000 )
		{
			const pid_t tid = current_thread().id();
			
			mark_thread_active( tid );
			
			thread_yield();
			
			mark_thread_inactive( tid );
			
			the_last_breath_time = clock();
			
			// Check for fatal signals again
			return check_signals( may_throw );
		}
		
		return false;
	}
	
}
