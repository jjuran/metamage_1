/*
	sigprocmask.cc
	--------------
*/

#include "relix/syscall/sigprocmask.hh"

// POSIX
#include <errno.h>
#include <signal.h>

// relix-api
#include "relix/api/errno.hh"

// relix-kernel
#include "relix/api/current_thread.hh"
#include "relix/task/thread.hh"


namespace relix
{
	
#if defined( __RELIX__ )  ||  defined( __APPLE__ )
	
	inline unsigned sigset_from_signo( int signo )
	{
		return 1 << (signo - 1);
	}
	
	int sigprocmask( int how, const sigset_t* set, sigset_t* oldset )
	{
		thread& current = current_thread();
		
		if ( oldset != 0 )  // NULL
		{
			*oldset = current.signals_blocked();
		}
		
		if ( set != 0 )  // NULL
		{
			const sigset_t unblockable_mask = sigset_from_signo( SIGKILL )
			                                | sigset_from_signo( SIGSTOP );
			
			const sigset_t filtered_set = *set & ~unblockable_mask;
			
			switch ( how )
			{
				case SIG_SETMASK:
					current.set_signals_blocked( filtered_set );
					break;
				
				case SIG_BLOCK:
					current.block_signals( filtered_set );
					break;
				
				case SIG_UNBLOCK:
					current.unblock_signals( filtered_set );
					break;
				
				default:
					return set_errno( EINVAL );
			}
		}
		
		return 0;
	}
	
#endif
	
}

