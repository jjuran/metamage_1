/*	========
 *	mkdir.cc
 *	========
 */

// Standard C/C++
#include <cstdio>

// Standard C
#include <errno.h>

// POSIX
#include <sys/stat.h>

// more-posix
#include "more/perror.hh"


#pragma exceptions off


int main( int argc, char *const *argv )
{
	// Check for sufficient number of args
	if ( argc < 2 )
	{
		more::perror( "mkdir", "missing arguments", 0 );
		
		return 1;
	}
	
	// Try to make each directory.  Return whether any errors occurred.
	int fail = 0;
	
	for ( int index = 1;  index < argc;  ++index )
	{
		int result = mkdir( argv[ index ], 0700 );
		
		if ( result == -1 )
		{
			more::perror( "mkdir", argv[ index ] );
			
			fail++;
		}
	}
	
	return (fail == 0) ? 0 : 1;
}

