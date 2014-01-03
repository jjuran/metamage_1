/*
	signals.cc
	----------
*/

#include "Genie/api/signals.hh"

// Standard C
#include <errno.h>
#include <signal.h>

// poseven
#include "poseven/types/errno_t.hh"

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
	
}

namespace Genie
{
	
	namespace p7 = poseven;
	
	
	bool check_signals( bool may_throw )
	{
		Process& current = current_process();
		
		return current.HandlePendingSignals( may_throw );
	}
	
	void broken_pipe()
	{
		relix::raise( SIGPIPE );
		
		p7::throw_errno( EPIPE );
	}
	
}

