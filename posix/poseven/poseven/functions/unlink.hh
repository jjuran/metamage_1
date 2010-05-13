// unlink.hh
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_UNLINK_HH
#define POSEVEN_FUNCTIONS_UNLINK_HH

// POSIX
#include <unistd.h>

// iota
#include "iota/string_traits.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	inline void unlink( const char* path )
	{
		throw_posix_result( ::unlink( path ) );
	}
	
	template < class String >
	inline void unlink( const String& path )
	{
		unlink( iota::get_string_c_str( path ) );
	}
	
}

#endif

