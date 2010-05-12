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
#include "iota/strings.hh"

// poseven
#include "poseven/types/errno_t.hh"
#include "poseven/types/mode_t.hh"


namespace poseven
{
	
	inline void mkdir( const char* path, mode_t mode = _777 )
	{
		throw_posix_result( ::mkdir( path, mode ) );
	}
	
	template < class String >
	inline void mkdir( const String& path, mode_t mode = _777 )
	{
		mkdir( iota::get_string_c_str( path ), mode );
	}
	
}

#endif

