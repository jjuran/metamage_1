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
	
	typedef struct stat struct_stat;
	
	bool stat( const char* pathname, struct stat& sb );
	
	struct stat stat( const char* pathname );
	
	template < class String >
	inline bool stat( const String&  path,
	                  struct stat&   sb )
	{
		using iota::get_string_c_str;
		
		return stat( get_string_c_str( path ), sb );
	}
	
	template < class String >
	inline
	struct_stat stat( const String& path )
	{
		using iota::get_string_c_str;
		
		return stat( get_string_c_str( path ) );
	}
	
}

#endif
