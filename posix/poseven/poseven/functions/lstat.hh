// lstat.hh
// --------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_LSTAT_HH
#define POSEVEN_FUNCTIONS_LSTAT_HH

// POSIX
#include <sys/stat.h>

// iota
#include "iota/string_traits.hh"


namespace poseven
{
	
	bool lstat( const char* pathname, struct stat& sb );
	
	struct ::stat lstat( const char* pathname );
	
	template < class String >
	inline bool lstat( const String&  path,
	                   struct stat&   sb )
	{
		return lstat( iota::get_string_c_str( path ), sb );
	}
	
	template < class String >
	inline struct ::stat lstat( const String& path )
	{
		return lstat( iota::get_string_c_str( path ) );
	}
	
}

#endif

