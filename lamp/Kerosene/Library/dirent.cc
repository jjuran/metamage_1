/*	=========
 *	dirent.cc
 *	=========
 */

// Standard C
#include "errno.h"

// POSIX
#include "dirent.h"
#include "fcntl.h"
#include "unistd.h"


DIR* fdopendir( int fd )
{
	DIR* result = NULL;
	
	if ( fd < 0 )
	{
		errno = EBADF;
		
		return result;
	}
	
	try
	{
		result = new DIR;
		
		result->fd = fd;
		
		int set = fcntl( fd, F_SETFD, FD_CLOEXEC );
	}
	catch ( ... )
	{
		errno = ENOMEM;
	}
	
	return result;
}

DIR* opendir( const char* pathname )
{
	DIR* result = NULL;
	
	try
	{
		DIR* dir = new DIR;
		
		int fd = open( pathname, O_RDONLY | O_DIRECTORY | O_CLOEXEC );
		
		if ( fd == -1 )
		{
			delete dir;
		}
		else
		{
			dir->fd = fd;
			
			result = dir;
		}
	}
	catch ( ... )
	{
		errno = ENOMEM;
	}
	
	return result;
}

struct dirent* readdir( DIR* dir )
{
	struct dirent *const entry = &dir->entry;
	
	int got = getdents( dirfd( dir ), entry, sizeof (dirent) );
	
	if ( got <= 0 )
	{
		return NULL;
	}
	
	return entry;
}

int closedir( DIR* dir )
{
	int fd = dirfd( dir );
	
	delete dir;
	
	return close( fd );
}

void rewinddir( DIR* dir )
{
	(void) lseek( dirfd( dir ), 0, SEEK_SET );
}

long telldir( DIR* dir )
{
	return lseek( dirfd( dir ), 0, SEEK_CUR );
}

void seekdir( DIR* dir, long offset )
{
	(void) lseek( dirfd( dir ), offset, SEEK_SET );
}

int dirfd( DIR* dir )
{
	return dir->fd;
}

