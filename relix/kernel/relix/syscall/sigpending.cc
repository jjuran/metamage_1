/*
	sigpending.cc
	-------------
*/

#include "relix/syscall/sigpending.hh"

// relix-kernel
#include "relix/api/current_thread.hh"
#include "relix/task/thread.hh"


namespace relix
{
	
	int sigpending( sigset_t* oldset )
	{
		if ( oldset != NULL )
		{
			*oldset = current_thread().signals_pending();
		}
		
		return 0;
	}
	
}

