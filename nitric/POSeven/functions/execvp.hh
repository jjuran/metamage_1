// execvp.hh
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_EXECVP_HH
#define POSEVEN_FUNCTIONS_EXECVP_HH

// POSIX
#include <errno.h>
#include <unistd.h>


namespace poseven
{
	
	inline void execvp( const char* path, char const *const *argv )
	{
		(void) ::execvp( path, (char**) argv );
		
		::_exit( errno == ENOENT ? 127 : 126 );
	}
	
	inline void execvp( char const *const *argv )
	{
		execvp( argv[ 0 ], argv );
	}
	
}

#endif

