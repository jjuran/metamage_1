/*
	extended-api-set/part-2.cc
	--------------------------
	
	Joshua Juran
*/

#include "extended-api-set/part-2.h"

// Standard C
#include <errno.h>
#include <stdio.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// Extended API Set, part 2
#include "extended-api-set/dual_path_at.hh"
#include "extended-api-set/temporary_cwd.hh"


#if !defined( __LAMP__ ) && !defined( __linux__ )

DIR *fdopendir( int fd )
{
	char path[] = "/dev/fd/1234567890";
	
	sprintf( path + sizeof "/dev/fd/" - 1, "%d", fd );
	
	DIR* dir = opendir( path );
	
	close( fd );
	
	return dir;
}

int fstatat( int dirfd, const char* path, struct stat* sb, int flags )
{
	_temporary_cwd cwd( dirfd, path );
	
	if ( int failed = cwd.failed() )
	{
		return failed;
	}
	
	const bool follow = (flags & AT_SYMLINK_NOFOLLOW) == 0;
	
	return follow ? stat( path, sb ) : lstat( path, sb );
}

static int link_with_flags( const char* oldpath, const char* newpath, int flags )
{
	const bool follow = flags & AT_SYMLINK_FOLLOW;
	
	struct ::stat sb;
	
	if ( !follow  &&  lstat( oldpath, &sb ) == 0  &&  S_ISLNK( sb.st_mode ) )
	{
		errno = ENOSYS;
		
		return -1;
	}
	
	return link( oldpath, newpath );
}

int linkat( int olddirfd, const char* oldpath, int newdirfd, const char* newpath, int flags )
{
	return _dual_path_at( &link_with_flags, olddirfd, oldpath, newdirfd, newpath, flags );
}

int mkdirat( int dirfd, const char* path, mode_t mode )
{
	_temporary_cwd cwd( dirfd, path );
	
	if ( int failed = cwd.failed() )
	{
		return failed;
	}
	
	return mkdir( path, mode );
}

int openat( int dirfd, const char* path, int flags, mode_t mode )
{
	_temporary_cwd cwd( dirfd, path );
	
	if ( int failed = cwd.failed() )
	{
		return failed;
	}
	
	return open( path, flags, mode );
}

ssize_t readlinkat( int dirfd, const char *path, char *buffer, size_t buffer_size )
{
	_temporary_cwd cwd( dirfd, path );
	
	if ( int failed = cwd.failed() )
	{
		return failed;
	}
	
	return readlink( path, buffer, buffer_size );
}

int renameat( int olddirfd, const char* oldpath, int newdirfd, const char* newpath )
{
	return _dual_path_at( (dual_path_filing_op) &rename, olddirfd, oldpath, newdirfd, newpath );
}

int symlinkat( const char* target, int newdirfd, const char* newpath )
{
	_temporary_cwd cwd( newdirfd, newpath );
	
	if ( int failed = cwd.failed() )
	{
		return failed;
	}
	
	return symlink( target, newpath );
}

int unlinkat( int dirfd, const char* path, int flags )
{
	_temporary_cwd cwd( dirfd, path );
	
	if ( int failed = cwd.failed() )
	{
		return failed;
	}
	
	const bool removedir = flags & AT_REMOVEDIR;
	
	return removedir ? rmdir ( path )
	                 : unlink( path );
}

#endif

