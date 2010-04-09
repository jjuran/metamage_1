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

// poseven
#include "poseven/functions/stat.hh"
#include "poseven/types/at_flags_t.hh"
#include "poseven/types/errno_t.hh"
#include "poseven/types/fd_t.hh"


namespace poseven
{
	
	inline bool fstatat( fd_t           dirfd,
	                     const char    *path,
	                     struct stat&   sb,
	                     at_flags_t     flags = at_flags_t() )
	{
		int status = ::fstatat( dirfd, path, &sb, flags );
		
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
	
	inline struct stat fstatat( fd_t         dirfd,
	                            const char  *path,
	                            at_flags_t   flags = at_flags_t() )
	{
		struct stat sb;
		
		throw_posix_result( ::fstatat( dirfd, path, &sb, flags ) );
		
		return sb;
	}
	
	inline bool fstatat( fd_t                dirfd,
	                     const std::string&  path,
	                     struct stat&        sb,
	                     at_flags_t          flags = at_flags_t() )
	{
		return fstatat( dirfd, path.c_str(), sb, flags );
	}
	
	inline struct stat fstatat( fd_t                dirfd,
	                            const std::string&  path,
	                            at_flags_t          flags = at_flags_t() )
	{
		return fstatat( dirfd, path.c_str(), flags );
	}
	
}

#endif

