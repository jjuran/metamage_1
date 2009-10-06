// unlink.hh
// ---------

// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_RMDIR_HH
#define POSEVEN_FUNCTIONS_RMDIR_HH

// Standard C++
#include <string>

// POSIX
#include <unistd.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	inline void rmdir( const char* path )
	{
		throw_posix_result( ::rmdir( path ) );
	}
	
	inline void rmdir( const std::string& path )
	{
		rmdir( path.c_str() );
	}
	
}

#endif

