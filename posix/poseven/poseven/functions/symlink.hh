// symlink.hh
// ----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_SYMLINK_HH
#define POSEVEN_FUNCTIONS_SYMLINK_HH

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	inline void symlink( const char* from, const char* to )
	{
		throw_posix_result( ::symlink( from, to ) );
	}
	
}

#endif

