/*
	sigpending.cc
	-------------
*/

#include "relix/syscall/sigpending.hh"

// relix-kernel
#include "relix/api/current_thread.hh"
#include "relix/task/process.hh"
#include "relix/task/thread.hh"


namespace relix
{
	
	int sigpending( sigset_t* oldset )
	{
		if ( oldset != 0 )  // NULL
		{
			thread&  self = current_thread();
			process& proc = self.get_process();
			
		#ifndef __linux__
			
			*oldset = self.signals_pending()
			        | proc.signals_pending();
			
		#endif
		}
		
		return 0;
	}
	
}
