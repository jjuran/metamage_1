/*	=======
 *	kill.cc
 *	=======
 */

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

// POSIX
#include <unistd.h>


#pragma export on

int main( int argc, char const *const argv[] )
{
	int sig = SIGTERM;
	
	char const *const *argp = argv;
	
	if ( argc > 1  &&  argp[ 1 ][ 0 ] == '-' )
	{
		sig = std::atoi( argp[ 1 ] + 1 );
		
		++argp;
		--argc;
	}
	
	if ( argc != 2 )
	{
		const char usage[] = "kill: usage: kill [-sig] pid\n";
		
		(void) write( STDERR_FILENO, usage, sizeof usage - 1 );
		
		return 1;
	}
	
	int pid = std::atoi( argp[ 1 ] );
	
	int killed = kill( pid, sig );
	
	if ( killed == -1 )
	{
		std::fprintf( stderr, "%s: %s: %s\n", argv[0], argp[1], std::strerror( errno ) );
		
		return 1;
	}
	
	return 0;
}

#pragma export reset

