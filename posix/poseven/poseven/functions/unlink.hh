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

// iota
#include "iota/string_traits.hh"


namespace poseven
{
	
	void unlink( const char* path );
	
	template < class String >
	inline void unlink( const String& path )
	{
		unlink( iota::get_string_c_str( path ) );
	}
	
}

#endif

