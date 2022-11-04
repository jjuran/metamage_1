/*
	setsid.cc
	---------
*/

#include "relix/syscall/setsid.hh"

// relix-kernel
#include "relix/api/current_process.hh"
#include "relix/api/errno.hh"
#include "relix/api/get_session_process_group.hh"
#include "relix/task/process.hh"
#include "relix/task/process_group.hh"
#include "relix/task/session.hh"


namespace relix
{
	
	int setsid()
	{
		try
		{
			process& current = current_process();
			
			const int pid = current.id();
			
			boost::intrusive_ptr< session > new_session = new session( pid );
			
			// throws EPERM if pgid already exists
			current.set_process_group( *get_session_process_group( pid, *new_session ) );
			
			return pid;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
}
