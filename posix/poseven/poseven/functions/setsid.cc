/*
	setsid.cc
	---------
*/

#include "poseven/functions/setsid.hh"

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	pid_t setsid()
	{
		return pid_t( throw_posix_result( ::setsid() ) );
	}
	
}

