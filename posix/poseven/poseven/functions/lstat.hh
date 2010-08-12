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

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	inline bool lstat( const char* pathname, struct stat& sb )
	{
		int status = ::lstat( pathname, &sb );
		
		if ( status == -1 )
		{
			if ( errno == ENOENT  ||  errno == ENOTDIR )
			{
				return false;
			}
			
			throw_errno( errno );
		}
		
		return true;
	}
	
	inline struct stat lstat( const char* pathname )
	{
		struct stat sb;
		
		throw_posix_result( ::lstat( pathname, &sb ) );
		
		return sb;
	}
	
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

