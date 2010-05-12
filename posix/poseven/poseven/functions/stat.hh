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
#include "iota/strings.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	inline bool stat( const char* pathname, struct stat& sb )
	{
		int status = ::stat( pathname, &sb );
		
		if ( status == -1 )
		{
			if ( errno == ENOENT )
			{
				return false;
			}
			
			throw_errno( errno );
		}
		
		return true;
	}
	
	inline struct stat stat( const char* pathname )
	{
		struct stat sb;
		
		throw_posix_result( ::stat( pathname, &sb ) );
		
		return sb;
	}
	
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

