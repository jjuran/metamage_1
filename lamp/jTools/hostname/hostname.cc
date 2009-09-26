/*	===========
 *	hostname.cc
 *	===========
 */

// Standard C
#include <string.h>

// POSIX
#include <unistd.h>

// more-posix
#include "more/perror.hh"


#pragma exceptions off


int main( int argc, char *const argv[] )
{
	if ( argc != 1 )
	{
		const char usage[] = "Usage: hostname\n";
		
		(void) write( STDERR_FILENO, usage, sizeof usage - 1 );
		
		return 1;
	}
	
	char hostname[ 256 ];
	
	int got = gethostname( hostname, sizeof hostname );
	
	if ( got < 0 )
	{
		more::perror( "hostname: gethostname" );
		
		return 1;
	}
	
	size_t length = strlen( hostname );
	
	hostname[ length++ ] = '\n';
	
	int written = write( STDOUT_FILENO, hostname, length );
	
	if ( written < 0 )
	{
		more::perror( "hostname: write" );
		
		return 1;
	}
	
	return 0;
}

