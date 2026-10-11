/*
	must/read.c
	-----------
*/

#include "must/read.h"

// POSIX
#include <unistd.h>


#define STR_LEN( s )  "" s, (sizeof s - 1)


ssize_t must_read( int fd, void* buffer, size_t n )
{
	const ssize_t n_read = read( fd, buffer, n );
	
	if ( n_read != n )
	{
		write( STDERR_FILENO, STR_LEN( "*** critical read() failed ***\n" ) );
		
		_exit( 13 );
	}
	
	return n_read;
}
