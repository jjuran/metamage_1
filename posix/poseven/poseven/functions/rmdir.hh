// unlink.hh
// ---------

// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_RMDIR_HH
#define POSEVEN_FUNCTIONS_RMDIR_HH

// POSIX
#include <unistd.h>

// iota
#include "iota/strings.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	inline void rmdir( const char* path )
	{
		throw_posix_result( ::rmdir( path ) );
	}
	
	template < class String >
	inline void rmdir( const String& path )
	{
		rmdir( iota::get_string_c_str( path ) );
	}
	
}

#endif

