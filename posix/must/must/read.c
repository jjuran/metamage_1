/*
	must/read.c
	-----------
*/

#include "must/read.h"

// Standard C
#include <stdlib.h>

// POSIX
#include <unistd.h>


ssize_t must_read( int fd, void* buffer, size_t n )
{
	const ssize_t n_read = read( fd, buffer, n );
	
	if ( n_read != n )
	{
		abort();
	}
	
	return n_read;
}
