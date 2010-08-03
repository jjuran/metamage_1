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

// Extended API Set, part 2
#include "extended-api-set/temporary_cwd.hh"


#if !defined( __LAMP__ ) && !defined( __linux__ )

static int get_path( int dirfd, const char* path, char* buffer, bool creating = false )
{
	ssize_t result = 0;
	
	if ( dirfd != AT_FDCWD  &&  path[0] != '/' )
	{
		int saved_cwd = open( ".", O_RDONLY );
		
		if ( saved_cwd < 0 )
		{
			return saved_cwd;
		}
		
		result = fchdir( dirfd );
		
		int saved_errno = errno;
		
		if ( result == 0 )
		{
			const bool made_dir = creating  &&  0 == mkdir( path, 0000 );  // so realpath() doesn't fail
			
			result = realpath( path, buffer ) ? 1 : -1;
			
			saved_errno = errno;
			
			if ( made_dir )
			{
				rmdir( path );
			}
		}
		
		fchdir( saved_cwd );
		
		close( saved_cwd );
		
		errno = saved_errno;
	}
	
	return result;
}

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

int linkat( int olddirfd, const char* oldpath, int newdirfd, const char* newpath, int flags )
{
	char old_pathname[ PATH_MAX ];
	char new_pathname[ PATH_MAX ];
	
	int got_old = get_path( olddirfd, oldpath, old_pathname );
	
	if ( got_old < 0 )
	{
		return got_old;
	}
	else if ( got_old )
	{
		oldpath = old_pathname;
	}
	
	int got_new = get_path( newdirfd, newpath, new_pathname, true );
	
	if ( got_new < 0 )
	{
		return got_new;
	}
	else if ( got_new )
	{
		newpath = new_pathname;
	}
	
	const bool follow = flags & AT_SYMLINK_FOLLOW;
	
	struct ::stat sb;
	
	if ( !follow  &&  lstat( oldpath, &sb ) == 0  &&  S_ISLNK( sb.st_mode ) )
	{
		errno = ENOSYS;
		
		return -1;
	}
	
	return link( oldpath, newpath );
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

int renameat( int olddirfd, const char* oldpath, int newdirfd, const char* newpath )
{
	char old_pathname[ PATH_MAX ];
	char new_pathname[ PATH_MAX ];
	
	int got_old = get_path( olddirfd, oldpath, old_pathname );
	
	if ( got_old < 0 )
	{
		return got_old;
	}
	else if ( got_old )
	{
		oldpath = old_pathname;
	}
	
	int got_new = get_path( newdirfd, newpath, new_pathname, true );
	
	if ( got_new < 0 )
	{
		return got_new;
	}
	else if ( got_new )
	{
		newpath = new_pathname;
	}
	
	return rename( oldpath, newpath );
}

int symlinkat( const char* target, int newdirfd, const char* newpath )
{
	char new_pathname[ PATH_MAX ];
	
	int got_new = get_path( newdirfd, newpath, new_pathname, true );
	
	if ( got_new < 0 )
	{
		return got_new;
	}
	else if ( got_new )
	{
		newpath = new_pathname;
	}
	
	return symlink( target, newpath );
}

int unlinkat( int dirfd, const char* path, int flags )
{
	char pathname[ PATH_MAX ];
	
	int got = get_path( dirfd, path, pathname );
	
	if ( got < 0 )
	{
		return got;
	}
	else if ( got )
	{
		path = pathname;
	}
	
	const bool removedir = flags & AT_REMOVEDIR;
	
	return removedir ? rmdir ( path )
	                 : unlink( path );
}

#endif

