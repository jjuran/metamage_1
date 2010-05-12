/*
	unlinkat.hh
	-----------
	
	Joshua Juran
	
	This code was written entirely by the above contributor, who places it
	in the public domain.
*/

#ifndef POSEVEN_FUNCTIONS_UNLINKAT_HH
#define POSEVEN_FUNCTIONS_UNLINKAT_HH

// POSIX
#include <unistd.h>

// iota
#include "iota/strings.hh"

// poseven
#include "poseven/types/at_flags_t.hh"
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	inline void unlinkat( fd_t         dirfd,
	                      const char*  path,
	                      at_flags_t   flags = at_flags_t() )
	{
		throw_posix_result( ::unlinkat( dirfd, path, flags ) );
	}
	
	template < class String >
	inline void unlinkat( const String& path )
	{
		unlinkat( iota::get_string_c_str( path ) );
	}
	
}

#endif

