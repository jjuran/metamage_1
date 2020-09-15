/*
	broken_pipe.cc
	--------------
*/

#include "relix/api/broken_pipe.hh"

// Standard C
#include <errno.h>
#include <signal.h>

// poseven
#include "poseven/types/errno_t.hh"

// relix-kernel
#include "relix/api/current_thread.hh"
#include "relix/task/thread.hh"


namespace relix
{
	
	namespace p7 = poseven;
	
	
	void broken_pipe()
	{
		current_thread().set_pending_signal( SIGPIPE );
		
		p7::throw_errno( EPIPE );
	}
	
}
