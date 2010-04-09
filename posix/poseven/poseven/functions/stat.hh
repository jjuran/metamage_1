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

// Standard C++
#include <string>

// POSIX
#include <sys/stat.h>

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
	
	inline bool stat( const std::string& pathname, struct stat& sb )
	{
		return stat( pathname.c_str(), sb );
	}
	
	inline struct stat stat( const std::string& pathname )
	{
		return stat( pathname.c_str() );
	}
	
}

#endif

