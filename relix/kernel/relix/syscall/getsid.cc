/*
	getsid.cc
	---------
*/

#include "relix/syscall/getsid.hh"

// relix-kernel
#include "relix/api/current_process.hh"
#include "relix/api/get_process.hh"
#include "relix/task/process.hh"
#include "relix/task/process_group.hh"
#include "relix/task/session.hh"


namespace relix
{
	
	int getsid( int pid )
	{
		const process& proc = pid ? get_process( pid ) : current_process();
		
		return proc.get_process_group().get_session().id();
	}
	
}

