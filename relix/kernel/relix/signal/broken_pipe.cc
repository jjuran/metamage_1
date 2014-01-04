/*
	broken_pipe.cc
	--------------
*/

#include "relix/signal/broken_pipe.hh"

// Standard C
#include <errno.h>
#include <signal.h>

// poseven
#include "poseven/types/errno_t.hh"

// relix-kernel
#include "relix/api/raise.hh"


namespace relix
{
	
	namespace p7 = poseven;
	
	
	void broken_pipe()
	{
		raise( SIGPIPE );
		
		p7::throw_errno( EPIPE );
	}
	
}

