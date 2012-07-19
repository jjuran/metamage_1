/*
	must/write.c
	------------
*/

#include "must/write.h"

// Standard C
#include <stdlib.h>

// POSIX
#include <unistd.h>


ssize_t must_write( int fd, const void* buffer, size_t n )
{
	const ssize_t written = write( fd, buffer, n );
	
	if ( written != n )
	{
		abort();
	}
	
	return written;
}

