/*
	yield.cc
	--------
*/

// relix-kernel
#include "relix/api/thread_yield.hh"
#include "relix/api/yield.hh"
#include "relix/task/scheduler.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/api/signals.hh"


namespace relix
{
	
	bool yield( bool may_throw )
	{
		using namespace Genie;
		
	#ifdef __RELIX__
		
		thread_yield();
		
	#else
		
		Process* current = relix::gCurrentProcess;
		
		pthread_yield();
		
		relix::gCurrentProcess = current;
		
	#endif
		
		return check_signals( may_throw );
	}
	
}
