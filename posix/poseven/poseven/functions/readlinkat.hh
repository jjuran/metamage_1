/*
	readlinkat.hh
	-------------
	
	Copyright 2010, Joshua Juran.
*/

#ifndef POSEVEN_FUNCTIONS_READLINKAT_HH
#define POSEVEN_FUNCTIONS_READLINKAT_HH

// Standard C++
#include <string>

// iota
#include "iota/strings.hh"

// poseven
#include "poseven/types/fd_t.hh"


namespace poseven
{
	
	std::string readlinkat( fd_t dirfd, const char* path );
	
	template < class String >
	inline std::string readlinkat( fd_t dirfd, const String& path )
	{
		return readlinkat( dirfd, iota::get_string_c_str( path ) );
	}
	
}

#endif

