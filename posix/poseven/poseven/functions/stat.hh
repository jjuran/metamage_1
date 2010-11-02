// stat.hh
// -------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_STAT_HH
#define POSEVEN_FUNCTIONS_STAT_HH

// POSIX
#include <sys/stat.h>

// iota
#include "iota/string_traits.hh"


namespace poseven
{
	
	bool stat( const char* pathname, struct stat& sb );
	
	struct stat stat( const char* pathname );
	
	template < class String >
	inline bool stat( const String&  path,
	                  struct stat&   sb )
	{
		return stat( iota::get_string_c_str( path ), sb );
	}
	
	template < class String >
	inline struct ::stat stat( const String& path )
	{
		return stat( iota::get_string_c_str( path ) );
	}
	
}

#endif

