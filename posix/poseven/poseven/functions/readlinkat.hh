/*
	readlinkat.hh
	-------------
	
	Copyright 2010, Joshua Juran.
*/

#ifndef POSEVEN_FUNCTIONS_READLINKAT_HH
#define POSEVEN_FUNCTIONS_READLINKAT_HH

// plus
#include "plus/string.hh"

// iota
#include "iota/string_traits.hh"

// poseven
#include "poseven/types/fd_t.hh"


namespace poseven
{
	
	plus::string readlinkat( fd_t dirfd, const char* path );
	
	template < class String >
	inline plus::string readlinkat( fd_t dirfd, const String& path )
	{
		return readlinkat( dirfd, iota::get_string_c_str( path ) );
	}
	
}

#endif

