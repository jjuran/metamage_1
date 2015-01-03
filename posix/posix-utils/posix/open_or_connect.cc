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
	
	int open_or_connect( const char* path, int flags, mode_t mode )
	{
		int fd = open( path, flags, mode );
		
		if ( fd < 0  &&  (errno == EOPNOTSUPP  ||  errno == ENXIO) )
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
