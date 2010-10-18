// symlink.hh
// ----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_SYMLINK_HH
#define POSEVEN_FUNCTIONS_SYMLINK_HH

// iota
#include "iota/string_traits.hh"


namespace poseven
{
	
	void symlink( const char* from, const char* to );
	
	template < class String1, class String2 >
	inline void symlink( const String1& from, const String2& to )
	{
		symlink( iota::get_string_c_str( from ), iota::get_string_c_str( to ) );
	}
	
}

#endif

