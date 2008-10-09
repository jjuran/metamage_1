// unlink.hh
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_UNLINK_HH
#define POSEVEN_FUNCTIONS_UNLINK_HH

// Standard C++
#include <string>

// POSIX
#include <unistd.h>

// POSeven
#include "POSeven/Errno.hh"


namespace poseven
{
	
	inline void unlink( const char* path )
	{
		throw_posix_result( ::unlink( path ) );
	}
	
	inline void unlink( const std::string& path )
	{
		unlink( path.c_str() );
	}
	
}

#endif

