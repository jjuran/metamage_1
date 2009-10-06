// fstatat.hh
// ----------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_FUNCTIONS_FSTATAT_HH
#define POSEVEN_FUNCTIONS_FSTATAT_HH

// Standard C++
#include <string>

// POSIX
#include <sys/stat.h>

// POSeven
#include "POSeven/types/errno_t.hh"


namespace poseven
{
	
	inline bool fstatat( fd_t dirfd, const char* path, struct stat& sb )
	{
		int status = ::fstatat( dirfd, path, &sb, 0 );
		
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
	
	inline struct stat fstatat( fd_t dirfd, const char* path )
	{
		struct stat sb;
		
		throw_posix_result( ::fstatat( dirfd, path, &sb, 0 ) );
		
		return sb;
	}
	
	inline bool fstatat( fd_t dirfd, const std::string& path, struct stat& sb )
	{
		return fstatat( dirfd, path.c_str(), sb );
	}
	
	inline struct stat fstatat( fd_t dirfd, const std::string& path )
	{
		return fstatat( dirfd, path.c_str() );
	}
	
}

namespace io
{
	
	// Existence
	
	inline bool item_exists( poseven::fd_t dirfd, const char* item, overload = overload() )
	{
		struct ::stat sb;
		
		return poseven::fstatat( dirfd, item, sb );
	}
	
	inline bool item_exists( poseven::fd_t dirfd, const std::string& item, overload = overload() )
	{
		return item_exists( dirfd, item.c_str() );
	}
	
	
	inline bool item_is_file( poseven::fd_t dirfd, const struct ::stat& sb, overload = overload() )
	{
		return S_ISREG( sb.st_mode );
	}
	
	inline bool item_is_directory( poseven::fd_t dirfd, const struct ::stat& sb, overload = overload() )
	{
		return S_ISDIR( sb.st_mode );
	}
	
	
	inline bool file_exists( poseven::fd_t dirfd, const char* item, overload = overload() )
	{
		struct ::stat sb;
		
		return poseven::fstatat( dirfd, item, sb ) && item_is_file( sb );
	}
	
	inline bool file_exists( poseven::fd_t dirfd, const std::string& item, overload = overload() )
	{
		return file_exists( dirfd, item.c_str() );
	}
	
	
	inline bool directory_exists( poseven::fd_t dirfd, const char* item, overload = overload() )
	{
		struct ::stat sb;
		
		return poseven::fstatat( dirfd, item, sb ) && item_is_directory( sb );
	}
	
	inline bool directory_exists( poseven::fd_t dirfd, const std::string& item, overload = overload() )
	{
		return directory_exists( dirfd, item.c_str() );
	}
	
}

#endif

