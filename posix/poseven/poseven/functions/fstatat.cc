/*
	fstatat.cc
	----------
*/

#include "poseven/functions/fstatat.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	bool fstatat( fd_t           dirfd,
	              const char    *path,
	              struct stat&   sb,
	              at_flags_t     flags )
	{
		int status = ::fstatat( dirfd, path, &sb, flags );
		
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
	
	struct stat fstatat( fd_t         dirfd,
	                     const char  *path,
	                     at_flags_t   flags )
	{
		struct stat sb;
		
		throw_posix_result( ::fstatat( dirfd, path, &sb, flags ) );
		
		return sb;
	}
	
}

