/*
	must/write.c
	------------
*/

#include "must/write.h"

// POSIX
#include <unistd.h>


#define STR_LEN( s )  "" s, (sizeof s - 1)


ssize_t must_write( int fd, const void* buffer, size_t n )
{
	const ssize_t written = write( fd, buffer, n );
	
	if ( written != n )
	{
		write( STDERR_FILENO, STR_LEN( "*** critical write() failed ***\n" ) );
		
		_exit( 13 );
	}
	
	return written;
}
