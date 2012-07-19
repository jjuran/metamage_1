/*
	must/pipe.c
	-----------
*/

#include "must/pipe.h"

// Standard C
#include <stdlib.h>

// POSIX
#include <unistd.h>


int must_pipe( int fds[2] )
{
	const int piped = pipe( fds );
	
	if ( piped != 0 )
	{
		abort();
	}
	
	return piped;
}

