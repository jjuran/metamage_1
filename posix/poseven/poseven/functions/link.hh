// link.hh
// -------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_LINK_HH
#define POSEVEN_FUNCTIONS_LINK_HH

// POSIX
#include <unistd.h>

// iota
#include "iota/strings.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	inline void link( const char* from, const char* to )
	{
		throw_posix_result( ::link( from, to ) );
	}
	
	template < class String1, class String2 >
	inline void link( const String1& from, const String2& to )
	{
		link( iota::get_string_c_str( from ), iota::get_string_c_str( to ) );
	}
	
}

#endif

