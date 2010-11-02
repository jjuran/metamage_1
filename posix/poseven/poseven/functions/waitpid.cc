/*
	waitpid.cc
	----------
*/

#include "poseven/functions/waitpid.hh"

// POSIX
#include <sys/wait.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	pid_t waitpid( pid_t pid, wait_t& status, int options )
	{
		return pid_t( throw_posix_result( ::waitpid( pid, (int*) &status, options ) ) );
	}
	
}

