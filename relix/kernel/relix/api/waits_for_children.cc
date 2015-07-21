/*
	waits_for_children.cc
	---------------------
*/

#include "relix/api/waits_for_children.hh"

// Standard C
#include <signal.h>

// relix-kernel
#include "relix/task/process.hh"


namespace relix
{
	
	bool waits_for_children( const process& proc )
	{
		const struct sigaction& chld = proc.get_sigaction( SIGCHLD );
		
		if ( chld.sa_handler == SIG_IGN )
		{
			return false;
		}
		
		enum
		{
			sa_nocldwait
			
		#ifdef SA_NOCLDWAIT
			
			= SA_NOCLDWAIT
			
		#endif
			
		};
		
		return (chld.sa_flags & sa_nocldwait) == 0;
	}
	
}
