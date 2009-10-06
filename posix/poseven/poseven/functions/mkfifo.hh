// mkfifo.hh
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_MKFIFO_HH
#define POSEVEN_FUNCTIONS_MKFIFO_HH

// Standard C++
#include <string>

// poseven
#include "poseven/types/errno_t.hh"
#include "poseven/types/mode_t.hh"


namespace poseven
{
	
	inline void mkfifo( const char* pathname, mode_t mode )
	{
		throw_posix_result( ::mkfifo( pathname, mode ) );
	}
	
	inline void mkfifo( const std::string& pathname, mode_t mode )
	{
		mkfifo( pathname.c_str(), mode );
	}
	
}

#endif

