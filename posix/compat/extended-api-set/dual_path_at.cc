/*
	extended-api-set/dual_path_at.cc
	--------------------------------
*/

#include "extended-api-set/dual_path_at.hh"

// Standard C
#include <limits.h>
#include <string.h>

// POSIX
#include <errno.h>
#include <unistd.h>

// Extended API Set, part 2
#include "extended-api-set/temporary_cwd.hh"


static inline int get_pathname( int dirfd, const char* path, char* buffer, size_t buffer_length )
{
	if ( *path == '\0' )
	{
		errno = ENOENT;
		
		return -1;
	}
	
	_temporary_cwd cwd( dirfd, path );
	
	if ( int failed = cwd.failed() )
	{
		return failed;
	}
	
	if ( getcwd( buffer, buffer_length ) == NULL )
	{
		return -1;
	}
	
	size_t pathname_length = strlen( buffer );
	
	const size_t path_len = strlen( path );
	
	if ( pathname_length + 1 + path_len + 1 > buffer_length )
	{
		errno = ENAMETOOLONG;
		
		return -1;
	}
	
	buffer[ pathname_length++ ] = '/';
	
	memcpy( &buffer[ pathname_length ], path, path_len );
	
	pathname_length += path_len;
	
	buffer[ pathname_length ] = '\0';
	
	path = buffer;
	
	return 0;
}


int _dual_path_at( dual_path_filing_op  op,
                   int                  olddirfd,
                   const char*          oldpath,
                   int                  newdirfd,
                   const char*          newpath,
                   int                  flags )
{
	const bool abs_old = oldpath[0] == '/';
	const bool abs_new = newpath[0] == '/';
	
	char pathname[ PATH_MAX + 1 ];
	
	if ( !abs_old  &&  !abs_new  &&  olddirfd != newdirfd )
	{
		if ( int failed = get_pathname( olddirfd, oldpath, pathname, sizeof pathname ) )
		{
			return failed;
		}
		
		oldpath = pathname;
	}
	
	const int   chdirfd = abs_new ? olddirfd : newdirfd;
	const char* chpath  = abs_new ? oldpath  : newpath;
	
	/*
		In the simple case, no chdir would happen for old or new, so which
		is selected doesn't matter.
		
		Otherwise, if one path is absolute then we chdir to the other.
		
		If neither path is absolute but the dirfds are the same, then it
		doesn't matter which one we select.
		
		If neither path was absolute and the dirfds are different, select
		the new one here (since the old path is now absolute).
	*/
	
	_temporary_cwd cwd( chdirfd, chpath );
	
	if ( int failed = cwd.failed() )
	{
		return failed;
	}
	
	return op( oldpath, newpath, flags );
}

