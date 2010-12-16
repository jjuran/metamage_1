/*
	breathe.cc
	----------
*/

#include "Genie/api/breathe.hh"

// Nitrogen
#include "Nitrogen/Timer.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"
#include "Genie/scheduler.hh"
#include "Genie/api/signals.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	bool breathe( bool may_throw )
	{
		if ( check_signals( may_throw ) )
		{
			return true;
		}
		
		const UInt64 now = N::Microseconds();
		
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

