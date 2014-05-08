/*
	yield.cc
	--------
*/

#include "Genie/api/yield.hh"

// relix-kernel
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
		
		current_process().Yield();
		
		return check_signals( may_throw );
	}
	
}

