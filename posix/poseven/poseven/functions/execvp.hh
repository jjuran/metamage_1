// execvp.hh
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_EXECVP_HH
#define POSEVEN_FUNCTIONS_EXECVP_HH

// iota
#include "iota/string_traits.hh"


namespace poseven
{
	
	void execvp( const char* path, char const *const *argv );
	
	inline void execvp( char const *const *argv )
	{
		execvp( argv[ 0 ], argv );
	}
	
	template < class String >
	inline void execvp( const String& path, char const *const *argv )
	{
		execvp( iota::get_string_c_str( path ), argv );
	}
	
}

#endif

