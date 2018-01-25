/*
	unstoppable.cc
	--------------
*/

#include "relix/signal/unstoppable.hh"

// relix-kernel
#include "relix/task/process.hh"
#include "relix/task/thread.hh"


namespace relix
{
	
	static inline
	bool has_default_sigaction( const process& proc, int signo )
	{
		return proc.get_sigaction( signo ).sa_handler == SIG_DFL;
	}
	
#if defined( __RELIX__ )  ||  defined( __APPLE__ )
	
	// GNU/Linux doesn't like the liberties we take with sigset_t.
	
	sigset_t unstoppable_signals( const thread& target )
	{
		const int signals[] =
		{
			SIGHUP,
			SIGINT,
			SIGQUIT,
		//	SIGKILL,
			SIGALRM,
			SIGTERM,
		//	SIGCONT,
		};
		
		sigset_t unstoppable = 1 << (SIGKILL - 1)
		                     | 1 << (SIGCONT - 1);
		
		const process& proc = target.get_process();
		
		const sigset_t unblocked = ~target.signals_blocked();
		
		for ( int i = 0;  i < sizeof signals / sizeof signals[ 0 ];  ++i )
		{
			const int signo = signals[ i ];
			
			const sigset_t mask = 1 << (signo - 1);
			
			if ( unblocked & mask  &&  has_default_sigaction( proc, signo ) )
			{
				unstoppable |= mask;
			}
		}
		
		return unstoppable;
	}
	
#endif
	
}
