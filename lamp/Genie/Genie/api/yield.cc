/*
	yield.cc
	--------
*/

// relix-kernel
#include "relix/api/yield.hh"
#include "relix/task/scheduler.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"
#include "Genie/api/signals.hh"


namespace relix
{
	
	bool yield( bool may_throw )
	{
		using namespace Genie;
		
		mark_thread_inactive( current_process().gettid() );
		
	#ifdef __RELIX__
		
		current_process().Yield();
		
	#else
		
		pthread_yield();
		
	#endif
		
		return check_signals( may_throw );
	}
	
}
