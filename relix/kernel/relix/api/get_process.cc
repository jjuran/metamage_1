/*
	get_process.cc
	--------------
*/

#include "relix/api/get_process.hh"

// poseven
#include "poseven/types/errno_t.hh"

// relix-api
#include "relix/api/get_thread.hh"

// relix-kernel
#include "relix/task/process.hh"
#include "relix/task/thread.hh"


namespace relix
{
	
	namespace p7 = poseven;
	
	
	process& get_process( int pid )
	{
		process& proc = get_thread( pid ).get_process();
		
		if ( proc.id() != pid )
		{
			// Fail if pid is not a process ID
			p7::throw_errno( ESRCH );
		}
		
		return proc;
	}
	
}

