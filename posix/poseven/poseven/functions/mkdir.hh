// mkdir.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_MKDIR_HH
#define POSEVEN_FUNCTIONS_MKDIR_HH

// POSIX
#include <unistd.h>

// iota
#include "iota/string_traits.hh"

// poseven
#ifndef POSEVEN_TYPES_MODE_T_HH
#include "poseven/types/mode_t.hh"
#endif


namespace poseven
{
	
	void mkdir( const char* path, mode_t mode = _777 );
	
	template < class String >
	inline void mkdir( const String& path, mode_t mode = _777 )
	{
		mkdir( iota::get_string_c_str( path ), mode );
	}
	
}

#endif

