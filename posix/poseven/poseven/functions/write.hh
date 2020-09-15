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

// POSIX
#include <sys/types.h>

// iota
#ifndef IOTA_ITERATOR_HH
#include "iota/iterator.hh"
#endif

// poseven
#ifndef POSEVEN_TYPES_FD_T_HH
#include "poseven/types/fd_t.hh"
#endif


namespace poseven
{
	
	ssize_t write( fd_t fd, const char* buffer, size_t bytes_requested );
	
	template < class String >
	inline ssize_t write( fd_t fd, const String& s )
	{
		using iota::data;
		using iota::size;
		
		return write( fd, data( s ), size( s ) );
	}
	
}

#endif
