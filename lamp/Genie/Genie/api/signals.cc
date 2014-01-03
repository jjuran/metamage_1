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

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	bool check_signals( bool may_throw )
	{
		Process& current = current_process();
		
		return current.HandlePendingSignals( may_throw );
	}
	
	static void send_signal_to_current_process( int signo )
	{
		current_process().Raise( signo );
	}
	
	void broken_pipe()
	{
		send_signal_to_current_process( SIGPIPE );
		
		p7::throw_errno( EPIPE );
	}
	
}

