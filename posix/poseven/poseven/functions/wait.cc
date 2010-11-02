/*
	wait.cc
	-------
*/

#include "poseven/functions/wait.hh"

// POSIX
#include <sys/wait.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	pid_t wait( wait_t& status )
	{
		return pid_t( throw_posix_result( ::wait( (int*) &status ) ) );
	}
	
}

