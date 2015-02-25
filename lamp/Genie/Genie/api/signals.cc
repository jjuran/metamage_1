/*
	signals.cc
	----------
*/

#include "Genie/api/signals.hh"

// relix-kernel
#include "relix/api/raise.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"


namespace relix
{
	
	void raise( int signo )
	{
		Genie::current_process().Raise( signo );
	}
	
	bool check_signals( bool may_throw )
	{
		using namespace Genie;
		
		Process& current = current_process();
		
		return current.HandlePendingSignals( may_throw );
	}
	
}
