/*	========
 *	rmdir.cc
 *	========
 */

// Standard C/C++
#include <cstdlib>

// POSIX
#include <unistd.h>

// Iota
#include "iota/strings.hh"

// more-posix
#include "more/perror.hh"


#pragma exceptions off


int main( int argc, char *const *argv )
{
	// Check for sufficient number of args
	if ( argc < 2 )
	{
		(void) write( STDERR_FILENO, STR_LEN( "rmdir: missing arguments\n" ) );
		
		return 1;
	}
	
	int exit_status = EXIT_SUCCESS;
	
	for ( int index = 1;  index < argc;  ++index )
	{
		const char* pathname = argv[ index ];
		
		int removed = rmdir( pathname );
		
		if ( removed == -1 )
		{
			exit_status = EXIT_FAILURE;
			
			more::perror( "rmdir", argv[ index ] );
		}
	}
	
	return exit_status;
}

