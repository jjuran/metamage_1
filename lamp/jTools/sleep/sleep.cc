/*	========
 *	sleep.cc
 *	========
 */

// Standard C/C++
#include <cstdio>

// POSIX
#include <time.h>
#include <unistd.h>

// Iota
#include "iota/strings.hh"


#pragma export on

int main( int argc, char const *const argv[] )
{
	// Check for correct number of args
	if ( argc != 2 )
	{
		write( STDERR_FILENO, STR_LEN( "Usage: sleep seconds\n" ) );
		
		return 2;
	}
	
	float sleep_time = 0.0;
	
	if ( int scanned = std::sscanf( argv[1], "%f", &sleep_time ) )
	{
		unsigned long seconds     = sleep_time;
		unsigned long nanoseconds = (sleep_time - seconds) * 1000 * 1000 * 1000;
		
		timespec time = { seconds, nanoseconds };
		
		nanosleep( &time, NULL );
	}
	else
	{
		std::fprintf( stderr, "sleep: bad argument '%s'\n", argv[1] );
		
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

#pragma export reset

