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

// POSeven
#include "POSeven/Errno.hh"


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

namespace io
{
	
	// Existence
	
	inline bool item_exists( const char* item )
	{
		struct ::stat sb;
		
		return poseven::stat( item, sb );
	}
	
	inline bool item_exists( const std::string& item )
	{
		return item_exists( item.c_str() );
	}
	
	
	inline bool item_is_file( const struct ::stat& sb )
	{
		return S_ISREG( sb.st_mode );
	}
	
	inline bool item_is_directory( const struct ::stat& sb )
	{
		return S_ISDIR( sb.st_mode );
	}
	
	
	inline bool file_exists( const char* item )
	{
		struct ::stat sb;
		
		return poseven::stat( item, sb ) && item_is_file( sb );
	}
	
	inline bool file_exists( const std::string& item )
	{
		return file_exists( item.c_str() );
	}
	
	
	inline bool directory_exists( const char* item )
	{
		struct ::stat sb;
		
		return poseven::stat( item, sb ) && item_is_directory( sb );
	}
	
	inline bool directory_exists( const std::string& item )
	{
		return directory_exists( item.c_str() );
	}
	
}

#endif

