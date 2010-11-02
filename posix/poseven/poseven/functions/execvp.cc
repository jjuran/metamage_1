/*
	execvp.cc
	---------
*/

#include "poseven/functions/execvp.hh"

// POSIX
#include <errno.h>
#include <unistd.h>


namespace poseven
{
	
	void execvp( const char* path, char const *const *argv )
	{
		(void) ::execvp( path, (char**) argv );
		
		::_exit( errno == ENOENT ? 127 : 126 );
	}
	
}

