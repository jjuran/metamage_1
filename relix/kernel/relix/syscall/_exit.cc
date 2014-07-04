/*
	_exit.cc
	--------
*/

#include "relix/syscall/_exit.hh"

// relix-kernel
#include "relix/api/terminate_current_process.hh"


namespace relix
{
	
	void _exit( int exit_status )
	{
		const int wait_status = (exit_status & 0xFF) << 8;
		
		terminate_current_process( wait_status );
	}
	
}
