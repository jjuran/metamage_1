// chdir.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_CHDIR_HH
#define POSEVEN_FUNCTIONS_CHDIR_HH

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	inline void chdir( const char* path )
	{
		throw_posix_result( ::chdir( path ) );
	}
	
}

#endif

