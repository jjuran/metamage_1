/*	===========
 *	hostname.cc
 *	===========
 */

// Standard C
#include <string.h>

// POSIX
#include <unistd.h>

// iota
#include "iota/strings.hh"

// more-posix
#include "more/perror.hh"


#pragma exceptions off


int main( int argc, char *const argv[] )
{
	if ( argc != 1 )
	{
		(void) write( STDERR_FILENO, STR_LEN( "Usage: hostname\n" ) );
		
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

