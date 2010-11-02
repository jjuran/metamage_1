/*
	lstat.cc
	--------
*/

#include "poseven/functions/lstat.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	bool lstat( const char* pathname, struct stat& sb )
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
	
	struct stat lstat( const char* pathname )
	{
		struct stat sb;
		
		throw_posix_result( ::lstat( pathname, &sb ) );
		
		return sb;
	}
	
}

