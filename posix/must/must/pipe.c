/*
	must/pipe.c
	-----------
*/

#include "must/pipe.h"

// POSIX
#include <unistd.h>


#define STR_LEN( s )  "" s, (sizeof s - 1)


int must_pipe( int fds[2] )
{
	const int piped = pipe( fds );
	
	if ( piped != 0 )
	{
		write( STDERR_FILENO, STR_LEN( "*** critical pipe() failed ***\n" ) );
		
		_exit( 13 );
	}
	
	return piped;
}
