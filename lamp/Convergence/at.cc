/*	=====
 *	at.cc
 *	=====
 */

#include "at.h"

// Standard C++
#include <algorithm>

// POSIX
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <utime.h>
#include <sys/stat.h>
#include <sys/time.h>


#ifndef __LAMP__

int futimesat_k( int dirfd, const char* path, const struct timeval* access,
                                              const struct timeval* mod,
                                              const struct timeval* backup,
                                              const struct timeval* creat )
{
	if ( backup  ||  creat  ||  (access == NULL) != (mod == NULL) )
	{
		errno = EINVAL;
		
		return -1;
	}
	
	int saved_cwd;
	
	int result = 0;
	
	const bool need_chdir = path[0] != '/';
	
	if ( need_chdir )
	{
		saved_cwd = open( ".", O_RDONLY );
		
		result = fchdir( dirfd );
		
		if ( result == -1 )
		{
			return result;
		}
	}
	
	if ( access == NULL  &&  mod == NULL )
	{
		result = utime( path, NULL );
	}
	else
	{
		const struct timeval times[] = { *access, *mod };
		
		result = utimes( path, times );
	}
	
	if ( need_chdir )
	{
		fchdir( saved_cwd );
		close ( saved_cwd );
	}
	
	return result;
}

#ifndef __linux__

DIR *fdopendir( int fd )
{
	char path[] = "/dev/fd/1234567890";
	
	std::sprintf( path + sizeof "/dev/fd/" - 1, "%d", fd );
	
	DIR* dir = opendir( path );
	
	close( fd );
	
	return dir;
}

int fstatat( int dirfd, const char* path, struct stat* sb, int flags )
{
	int saved_cwd;
	
	int result = 0;
	
	const bool need_chdir = path[0] != '/';
	
	if ( need_chdir )
	{
		saved_cwd = open( ".", O_RDONLY );
		
		result = fchdir( dirfd );
		
		if ( result == -1 )
		{
			return result;
		}
	}
	
	const bool follow = true;  // FIXME
	
	result = follow ? stat( path, sb ) : lstat( path, sb );
	
	if ( need_chdir )
	{
		fchdir( saved_cwd );
		close ( saved_cwd );
	}
	
	return result;
}

int mkdirat( int dirfd, const char* path, mode_t mode )
{
	int saved_cwd;
	
	int result = 0;
	
	const bool need_chdir = path[0] != '/';
	
	if ( need_chdir )
	{
		saved_cwd = open( ".", O_RDONLY );
		
		result = fchdir( dirfd );
		
		if ( result == -1 )
		{
			return result;
		}
	}
	
	result = mkdir( path, mode );
	
	if ( need_chdir )
	{
		fchdir( saved_cwd );
		close ( saved_cwd );
	}
	
	return result;
}

int openat( int dirfd, const char* path, int flags, mode_t mode )
{
	int saved_cwd;
	
	int result = 0;
	
	const bool need_chdir = path[0] != '/';
	
	if ( need_chdir )
	{
		saved_cwd = open( ".", O_RDONLY );
		
		result = fchdir( dirfd );
		
		if ( result == -1 )
		{
			return result;
		}
	}
	
	result = open( path, flags, mode );
	
	if ( need_chdir )
	{
		fchdir( saved_cwd );
		close ( saved_cwd );
	}
	
	return result;
}

#endif

#endif

