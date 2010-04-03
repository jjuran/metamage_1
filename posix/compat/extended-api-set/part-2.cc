/*
	extended-api-set/part-2.cc
	--------------------------
	
	Joshua Juran
*/

#include "extended-api-set/part-2.h"

// Standard C++
#include <algorithm>

// POSIX
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>


#if !defined( __LAMP__ ) && !defined( __linux__ )

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
	
	const bool follow = (flags & AT_SYMLINK_NOFOLLOW) == 0;
	
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

ssize_t readlinkat( int dirfd, const char *path, char *buffer, size_t buffer_size )
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
	
	result = readlink( path, buffer, buffer_size );
	
	if ( need_chdir )
	{
		fchdir( saved_cwd );
		close ( saved_cwd );
	}
	
	return result;
}

#endif

