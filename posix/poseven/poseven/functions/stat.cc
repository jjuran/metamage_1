/*
	stat.cc
	-------
*/

#include "poseven/functions/stat.hh"

// POSIX
#include <sys/stat.h>

// poseven
#include "poseven/types/errno_t.hh"


namespace poseven
{
	
	bool stat( const char* pathname, struct stat& sb )
	{
		int status = ::stat( pathname, &sb );
		
		if ( status < 0 )
		{
			if ( errno == ENOENT  ||  errno == ENOTDIR )
			{
				sb.st_mode = 0;
				
				return false;
			}
			
			throw_errno( errno );
		}
		
		return true;
	}
	
	struct stat stat( const char* pathname )
	{
		struct stat sb;
		
		throw_posix_result( ::stat( pathname, &sb ) );
		
		return sb;
	}
	
}
