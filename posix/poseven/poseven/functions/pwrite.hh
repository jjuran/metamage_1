// pwrite.hh
// ---------

// Written 2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_PWRITE_HH
#define POSEVEN_FUNCTIONS_PWRITE_HH

// POSIX
#include <sys/types.h>

// poseven
#include "poseven/types/fd_t.hh"


namespace poseven
{
	
	ssize_t pwrite( fd_t fd, const char* buffer, size_t n_bytes, off_t offset );
	
	template < class String >
	inline ssize_t pwrite( fd_t fd, const String& string, off_t offset )
	{
		return pwrite( fd, string.data(), string.length(), offset );
	}
	
}

#endif

