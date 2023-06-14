/*	========
 *	rmdir.cc
 *	========
 */

// POSIX
#include <unistd.h>

// must
#include "must/write.h"

// more-posix
#include "more/perror.hh"


#pragma exceptions off


#define STR_LEN( s ) "" s, (sizeof s - 1)


int main( int argc, char *const *argv )
{
	// Check for sufficient number of args
	if ( argc < 2 )
	{
		must_write( STDERR_FILENO, STR_LEN( "rmdir: missing arguments\n" ) );
		
		return 1;
	}
	
	int exit_status = 0;
	
	for ( int index = 1;  index < argc;  ++index )
	{
		const char* pathname = argv[ index ];
		
		int removed = rmdir( pathname );
		
		if ( removed == -1 )
		{
			exit_status = 1;
			
			more::perror( "rmdir", argv[ index ] );
		}
	}
	
	return exit_status;
}
