/*
	signals.cc
	----------
*/

#include "Genie/api/signals.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"


namespace relix
{
	
	bool check_signals( bool may_throw )
	{
		using namespace Genie;
		
		Process& current = current_process();
		
		return current.HandlePendingSignals( may_throw );
	}
	
}
