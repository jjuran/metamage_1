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
	
	bool breathe( bool may_throw )
	{
		if ( check_signals( may_throw ) )
		{
			return true;
		}
		
		const uint64_t now = clock();
		
		Process& current = current_process();
		
		if ( now - current.GetTimeOfLastResume() > 20000 )
		{
			mark_process_active( current_process().GetPID() );
			
			current.Breathe();
			
			// Check for fatal signals again
			return check_signals( may_throw );
		}
		
		return false;
	}
	
}

