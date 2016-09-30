/*
	open_or_connect.cc
	------------------
*/

#include "posix/open_or_connect.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

// Standard C
#include <errno.h>
#include <string.h>


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
			sockaddr_un un = { 0 };
			
			un.sun_family = AF_LOCAL;
			
			const size_t length = strlen( path );
			
			if ( length >= sizeof un.sun_path )
			{
				errno = ENAMETOOLONG;
				
				return fd;
			}
			
			memcpy( un.sun_path, path, length + 1 );
			
			fd = socket( PF_LOCAL, SOCK_STREAM, 0 );
			
			if ( fd < 0 )
			{
				return fd;
			}
			
			int nok = connect( fd, (const sockaddr*) &un, sizeof un );
			
			if ( nok )
			{
				int saved_errno = errno;
				
				close( fd );
				
				errno = saved_errno;
				
				return -1;
			}
		}
		
		return fd;
	}
	
}
