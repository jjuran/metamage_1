// write.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_WRITE_HH
#define POSEVEN_FUNCTIONS_WRITE_HH

// Standard C++
#include <string>

// POSIX
#include <unistd.h>

// plus
#include "plus/string.hh"

// poseven
#include "poseven/types/errno_t.hh"
#include "poseven/types/fd_t.hh"


namespace poseven
{
	
	inline ssize_t write( fd_t fd, const char* buffer, std::size_t bytes_requested )
	{
		return throw_posix_result( ::write( fd, buffer, bytes_requested ) );
	}
	
	inline ssize_t write( fd_t fd, const plus::string& string )
	{
		return write( fd, string.data(), string.length() );
	}
	
	inline ssize_t write( fd_t fd, const std::string& string )
	{
		return write( fd, string.data(), string.length() );
	}
	
}

#endif

