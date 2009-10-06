// pwrite.hh
// ---------

// Written 2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_PWRITE_HH
#define POSEVEN_FUNCTIONS_PWRITE_HH

// Standard C++
#include <string>

// POSIX
#include <unistd.h>

// POSeven
#include "POSeven/types/errno_t.hh"
#include "POSeven/types/fd_t.hh"


namespace poseven
{
	
	inline ssize_t pwrite( fd_t fd, const char* buffer, size_t n_bytes, off_t offset )
	{
		return throw_posix_result( ::pwrite( fd, buffer, n_bytes, offset ) );
	}
	
	inline ssize_t pwrite( fd_t fd, const std::string& string, off_t offset )
	{
		return pwrite( fd, string.data(), string.length(), offset );
	}
	
}

#endif

