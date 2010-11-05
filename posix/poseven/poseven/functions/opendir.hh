// opendir.hh
// ----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_OPENDIR_HH
#define POSEVEN_FUNCTIONS_OPENDIR_HH

// iota
#include "iota/string_traits.hh"

// poseven
#ifndef POSEVEN_FUNCTIONS_CLOSEDIR_HH
#include "poseven/functions/closedir.hh"
#endif


namespace poseven
{
	
	nucleus::owned< dir_t > opendir( const char* pathname );
	
	template < class String >
	inline nucleus::owned< dir_t > opendir( const String& path )
	{
		return opendir( iota::get_string_c_str( path ) );
	}
	
}

#endif

