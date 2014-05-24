/*
	sigsuspend.cc
	-------------
*/

#include "relix/syscall/sigsuspend.hh"

// relix-kernel
#include "relix/api/current_thread.hh"
#include "relix/api/errno.hh"
#include "relix/api/yield.hh"
#include "relix/signal/caught_signal.hh"
#include "relix/task/thread.hh"


namespace relix
{
	
	int sigsuspend( const sigset_t* sigmask )
	{
		thread& this_thread = current_thread();
		
		const sigset_t previous = this_thread.signals_blocked();
		
		if ( sigmask != NULL )
		{
			this_thread.set_signals_blocked( *sigmask );
		}
		
		try
		{
			while ( true )
			{
				yield( true );  // throw caught signals
			}
		}
		catch ( ... )
		{
			(void) set_errno_from_exception();
		}
		
		this_thread.set_signals_blocked( previous );
		
		prevent_syscall_restart();
		
		return -1;  // EINTR
	}
	
}
