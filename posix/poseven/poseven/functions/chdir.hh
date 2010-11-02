// chdir.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_CHDIR_HH
#define POSEVEN_FUNCTIONS_CHDIR_HH

// iota
#include "iota/string_traits.hh"


namespace poseven
{
	
	void chdir( const char* path );
	
	template < class String >
	inline void chdir( const String& path )
	{
		chdir( iota::get_string_c_str( path ) );
	}
	
}

#endif

