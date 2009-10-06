// execv.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_EXECV_HH
#define POSEVEN_FUNCTIONS_EXECV_HH

// POSIX
#include <errno.h>
#include <unistd.h>


namespace poseven
{
	
	inline void execv( const char* pathname, char const *const *argv )
	{
		(void) ::execv( pathname, (char**) argv );
		
		::_exit( errno == ENOENT ? 127 : 126 );
	}
	
	inline void execv( char const *const *argv )
	{
		execv( argv[ 0 ], argv );
	}
	
}

#endif

