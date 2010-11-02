/*
	execv.cc
	--------
*/

#include "poseven/functions/execv.hh"

// POSIX
#include <errno.h>
#include <unistd.h>


namespace poseven
{
	
	void execv( const char* pathname, char const *const *argv )
	{
		(void) ::execv( pathname, (char**) argv );
		
		::_exit( errno == ENOENT ? 127 : 126 );
	}
	
}

