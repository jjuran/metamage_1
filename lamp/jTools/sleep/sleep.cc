/*	========
 *	sleep.cc
 *	========
 */

// Standard C/C++
#include <cstdlib>

// POSIX
#include "unistd.h"


#pragma export on

int main( int argc, char const *const argv[] )
{
	// Check for correct number of args
	if ( argc != 2 )
	{
		const char usage[] = "usage: sleep seconds\n";
		
		write( STDERR_FILENO, usage, sizeof usage - 1 );
		
		return 1;
	}
	
	unsigned int seconds = std::atoi( argv[ 1 ] );
	
	sleep( seconds );
	
	return 0;
}

#pragma export reset

