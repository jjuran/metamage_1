// mkdir.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_MKDIR_HH
#define POSEVEN_FUNCTIONS_MKDIR_HH

// Standard C++
#include <string>

// POSIX
#include <unistd.h>

// POSeven
#include "POSeven/types/errno_t.hh"
#include "POSeven/types/mode_t.hh"


namespace poseven
{
	
	inline void mkdir( const char* path, mode_t mode = _777 )
	{
		throw_posix_result( ::mkdir( path, mode ) );
	}
	
	inline void mkdir( const std::string& path, mode_t mode = _777 )
	{
		mkdir( path.c_str(), mode );
	}
	
}

#endif

