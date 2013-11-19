/*
	execve.cc
	---------
*/

#include "poseven/functions/execve.hh"

// POSIX
#include <errno.h>
#include <unistd.h>


namespace poseven
{
	
	void execve( const char* pathname, char const* const* argv, char const* const* envp )
	{
		(void) ::execve( pathname, (char**) argv, (char**) envp );
		
		::_exit( errno == ENOENT ? 127 : 126 );
	}
	
}

