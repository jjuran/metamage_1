/*
	bindir.cc
	---------
*/

#include "posix/bindir.hh"

// POSIX
#include <fcntl.h>

// Standard C
#include <string.h>

// compat
#include "extended-api-set/part-2.h"


int bindir( char* argv0 )
{
	int dirfd;
	
	if ( char* slash = strrchr( argv0, '/' ) )
	{
		*slash = '\0';
		
		dirfd = open( argv0, O_RDONLY | O_DIRECTORY );
		
		*slash = '/';
	}
	else
	{
		dirfd = open( ".", O_RDONLY | O_DIRECTORY );
	}
	
	return dirfd;
}
