/*
	sigaction.cc
	------------
*/

#include "relix/syscall/sigaction.hh"

// POSIX
#include <signal.h>

// Standard C
#include <errno.h>

// relix-kernel
#include "relix/api/current_thread.hh"
#include "relix/api/errno.hh"
#include "relix/task/process.hh"
#include "relix/task/thread.hh"


namespace relix
{
	
	int sigaction( int signo, const struct sigaction* action, struct sigaction* oldaction )
	{
		if ( signo <= 0  ||  signo >= NSIG )
		{
			return set_errno( EINVAL );
		}
		
		thread& this_thread = current_thread();
		
		process& this_process = this_thread.get_process();
		
		if ( oldaction != NULL )
		{
			*oldaction = this_process.get_sigaction( signo );
		}
		
		if ( action != NULL )
		{
			if ( signo == SIGKILL  ||  signo == SIGSTOP )
			{
				return set_errno( EINVAL );
			}
			
			if ( action->sa_handler == SIG_IGN )
			{
				this_thread.clear_pending_signal( signo );
			}
			
		#ifdef SA_SIGINFO
			
			if ( action->sa_flags & SA_SIGINFO )
			{
				return set_errno( ENOTSUP );
			}
			
		#endif
			
			this_process.set_sigaction( signo, *action );
		}
		
		return 0;
	}
	
}
