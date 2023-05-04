/*	=========
 *	dirent.cc
 *	=========
 */

// Standard C
#include "errno.h"
#include "stdlib.h"

// POSIX
#include "dirent.h"
#include "fcntl.h"
#include "unistd.h"


#pragma exceptions off


DIR* fdopendir( int fd )
{
	DIR* result = NULL;
	
	if ( fd < 0 )
	{
		errno = EBADF;
		
		return result;
	}
	
	if (( result = (DIR*) malloc( sizeof (DIR) ) ))
	{
		result->fd = fd;
		
		int set = fcntl( fd, F_SETFD, FD_CLOEXEC );
	}
	else
	{
		errno = ENOMEM;
	}
	
	return result;
}

DIR* opendir( const char* pathname )
{
	DIR* result = NULL;
	
	if ( DIR* dir = (DIR*) malloc( sizeof (DIR) ) )
	{
		int fd = open( pathname, O_RDONLY | O_DIRECTORY | O_CLOEXEC );
		
		if ( fd == -1 )
		{
			free( dir );
		}
		else
		{
			dir->fd = fd;
			
			result = dir;
		}
	}
	else
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
	
	free( dir );
	
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
