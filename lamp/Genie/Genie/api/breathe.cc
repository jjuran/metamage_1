/*
	breathe.cc
	----------
*/

#include "Genie/api/breathe.hh"

// Standard C
#include <time.h>

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"
#include "Genie/scheduler.hh"
#include "Genie/api/signals.hh"


namespace Genie
{
	
	static clock_t the_last_breath_time;
	
	
	bool breathe( bool may_throw )
	{
		if ( check_signals( may_throw ) )
		{
			return true;
		}
		
		const uint64_t now = clock();
		
		Process& current = current_process();
		
		if ( now - the_last_breath_time > 20000 )
		{
			mark_process_active( current_process().gettid() );
			
			current.Breathe();
			
			the_last_breath_time = clock();
			
			// Check for fatal signals again
			return check_signals( may_throw );
		}
		
		return false;
	}
	
}

