/*
	must/chdir.c
	------------
*/

#include "must/chdir.h"

// POSIX
#include <unistd.h>


#define STR_LEN( s )  "" s, (sizeof s - 1)


int must_chdir( const char* path )
{
	const int changed = chdir( path );
	
	if ( changed != 0 )
	{
		write( STDERR_FILENO, STR_LEN( "*** critical chdir() failed ***\n" ) );
		
		_exit( 13 );
	}
	
	return changed;
}
