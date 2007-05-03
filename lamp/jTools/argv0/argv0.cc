/*	========
 *	argv0.cc
 *	========
 */

// Standard C
#include <errno.h>
#include <stdio.h>

// POSIX
#include <unistd.h>


#pragma exceptions off


#pragma export on

int main( int argc, char const *const argv[] )
{
	if ( argc < 3 )
	{
		const char usage[] = "Usage: argv0 command arg0 [ arg1 ... argn ]\n";
		
		(void) write( STDERR_FILENO, usage, sizeof usage - 1 );
		
		return 1;
	}
	
	(void) execvp( argv[ 1 ], argv + 2 );
	
	std::fprintf( stderr, "%s: %s: %s\n", argv[0], argv[1], std::strerror( errno ) );
	
	return 127;
}

#pragma export reset

