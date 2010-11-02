// chmod.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_CHMOD_HH
#define POSEVEN_FUNCTIONS_CHMOD_HH

// iota
#include "iota/string_traits.hh"

// poseven
#include "poseven/types/mode_t.hh"


namespace poseven
{
	
	void chmod( const char* pathname, mode_t mode );
	
	template < class String >
	inline void chmod( const String& path, mode_t mode )
	{
		chmod( iota::get_string_c_str( path ), mode );
	}
	
}

#endif

