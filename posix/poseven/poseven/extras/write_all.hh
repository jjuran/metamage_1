/*
	write_all.hh
	------------
	
	Copyright 2009, Joshua Juran
*/


#ifndef POSEVEN_EXTRAS_WRITEALL_HH
#define POSEVEN_EXTRAS_WRITEALL_HH

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
	
	ssize_t write_some( fd_t fd, const char* buffer, size_t n_bytes );
	
	template < class String >
	inline ssize_t write_some( fd_t fd, const String& s )
	{
		using iota::data;
		using iota::size;
		
		return write_some( fd, data( s ), size( s ) );
	}
	
	ssize_t write_all( fd_t fd, const char* buffer, size_t n_bytes );
	
	template < class String >
	inline ssize_t write_all( fd_t fd, const String& s )
	{
		using iota::data;
		using iota::size;
		
		return write_all( fd, data( s ), size( s ) );
	}
	
}

#endif
