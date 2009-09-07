/*	========
 *	argv0.cc
 *	========
 */

// POSIX
#include <unistd.h>

// more-posix
#include "more/perror.hh"


#pragma exceptions off


int main( int argc, char *const argv[] )
{
	if ( argc < 3 )
	{
		const char usage[] = "Usage: argv0 command arg0 [ arg1 ... argn ]\n";
		
		(void) write( STDERR_FILENO, usage, sizeof usage - 1 );
		
		return 1;
	}
	
	(void) execvp( argv[ 1 ], argv + 2 );
	
	more::perror( argv[0], argv[1] );
	
	return 127;
}

