/*
	open_or_connect.cc
	------------------
*/

#include "posix/open_or_connect.hh"

// POSIX
#include <fcntl.h>

// Standard C
#include <errno.h>

// posix-utils
#include "posix/connect_path.hh"


namespace posix
{
	
	static
	bool might_be_socket( int error )
	{
	#ifdef __APPLE__
		
		/*
			Observed behavior on 10.4:
			EOPNOTSUPP is 45, but open() sets errno to 102.
			
			Look in /usr/include/sys/errno.h --  I'm not making this up.
		*/
		
		if ( error == 102 )
		{
			return true;
		}
		
	#endif
		
		return errno == EOPNOTSUPP  ||  errno == ENXIO;
	}
	
	int open_or_connect( const char* path, int flags, mode_t mode )
	{
		int fd = open( path, flags, mode );
		
		if ( fd < 0  &&  might_be_socket( errno ) )
		{
			fd = connect_path( path );
		}
		
		return fd;
	}
	
}
