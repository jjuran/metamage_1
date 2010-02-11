/*
	readlinkat.hh
	-------------
	
	Copyright 2010, Joshua Juran.
*/

#ifndef POSEVEN_FUNCTIONS_READLINKAT_HH
#define POSEVEN_FUNCTIONS_READLINKAT_HH

// Standard C++
#include <string>

// poseven
#include "poseven/types/fd_t.hh"


namespace poseven
{
	
	std::string readlinkat( fd_t dirfd, const char* path );
	
	inline std::string readlinkat( fd_t dirfd, const std::string& path )
	{
		return readlinkat( dirfd, path.c_str() );
	}
	
}

#endif

