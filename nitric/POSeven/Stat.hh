// Stat.hh
// -------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_STAT_HH
#define POSEVEN_STAT_HH

// POSIX
#include <sys/stat.h>

// Io
#include "io/io.hh"

// POSeven
#include "POSeven/Errno.hh"


namespace POSeven
{
	
	using ::stat;
	
	inline bool Stat( const char* name, struct stat& sb )
	{
		int status = ::stat( name, &sb );
		
		if ( status == -1 )
		{
			if ( errno == ENOENT )
			{
				return false;
			}
			
			ThrowErrno( errno );
		}
		
		return true;
	}
	
	inline struct stat Stat( const char* name )
	{
		struct stat sb;
		
		if ( !Stat( name, sb ) )
		{
			ThrowErrno( ENOENT );
		}
		
		return sb;
	}
	
	inline bool Stat( const std::string& name, struct stat& sb )
	{
		return Stat( name.c_str(), sb );
	}
	
	inline struct stat Stat( const std::string& name )
	{
		return Stat( name.c_str() );
	}
	
}

namespace io
{
	
	// Existence
	
	inline bool item_exists( const char* item )
	{
		struct ::stat sb;
		
		return POSeven::Stat( item, sb );
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
		
		return POSeven::Stat( item, sb ) && item_is_file( sb );
	}
	
	inline bool file_exists( const std::string& item )
	{
		return file_exists( item.c_str() );
	}
	
	
	inline bool directory_exists( const char* item )
	{
		struct ::stat sb;
		
		return POSeven::Stat( item, sb ) && item_is_directory( sb );
	}
	
	inline bool directory_exists( const std::string& item )
	{
		return directory_exists( item.c_str() );
	}
	
}

#endif

