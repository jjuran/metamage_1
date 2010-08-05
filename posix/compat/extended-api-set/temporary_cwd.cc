/*
	extended-api-set/temporary_cwd.cc
	---------------------------------
*/

#include "extended-api-set/part-2.h"
#include "extended-api-set/temporary_cwd.hh"

// POSIX
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>


_temporary_cwd::_temporary_cwd( int dirfd, const char* path )
:
	// no handling is needed if dirfd is cwd or path is absolute
	it_needs_chdir( dirfd != AT_FDCWD  &&  path[0] != '/' ),
	// save an fd for cwd (if needed)
	its_saved_cwd ( it_needs_chdir ? open( ".", O_RDONLY ) : -1 ),
	// -1 if open() or fchdir() failed
	it_has_failed ( it_needs_chdir && its_saved_cwd < 0  ?  -1  :  0 )
{
	if ( it_needs_chdir  &&  !it_has_failed )
	{
		it_has_failed = fchdir( dirfd );
	}
}

_temporary_cwd::~_temporary_cwd()
{
	if ( its_saved_cwd >= 0 )
	{
		const int saved_errno = errno;
		
		if ( !it_has_failed )
		{
			fchdir( its_saved_cwd );
		}
		
		close( its_saved_cwd );
		
		errno = saved_errno;
	}
}

