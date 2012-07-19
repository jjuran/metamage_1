/*
	must/chdir.c
	------------
*/

#include "must/chdir.h"

// Standard C
#include <stdlib.h>

// POSIX
#include <unistd.h>


int must_chdir( const char* path )
{
	const int changed = chdir( path );
	
	if ( changed != 0 )
	{
		abort();
	}
	
	return changed;
}

