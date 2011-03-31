/*
	write_all.hh
	------------
	
	Copyright 2009, Joshua Juran
*/


#ifndef POSEVEN_EXTRAS_WRITEALL_HH
#define POSEVEN_EXTRAS_WRITEALL_HH

// POSIX
#include <sys/types.h>

// plus
#ifndef PLUS_STRING_HH
#include "plus/string.hh"
#endif

// poseven
#ifndef POSEVEN_TYPES_FD_T_HH
#include "poseven/types/fd_t.hh"
#endif


namespace poseven
{
	
	ssize_t write_some( fd_t fd, const char* buffer, size_t n_bytes );
	
	inline ssize_t write_some( fd_t fd, const plus::string& string )
	{
		return write_some( fd,
		                   string.data(),
		                   string.size() );
	}
	
	ssize_t write_all( fd_t fd, const char* buffer, size_t n_bytes );
	
	inline ssize_t write_all( fd_t fd, const plus::string& string )
	{
		return write_all( fd,
		                  string.data(),
		                  string.size() );
	}
	
}

#endif

