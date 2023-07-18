/*	===========
 *	hostname.cc
 *	===========
 */

// Standard C
#include <string.h>

// POSIX
#include <unistd.h>

// must
#include "must/write.h"

// more-posix
#include "more/perror.hh"


#pragma exceptions off


#define STR_LEN( s )  "" s, (sizeof s - 1)


int main( int argc, char *const argv[] )
{
	if ( argc > 1  &&  strcmp( argv[1], "-s" ) == 0 )
	{
		--argc;
	}
	
	if ( argc != 1 )
	{
		must_write( STDERR_FILENO, STR_LEN( "Usage: hostname\n" ) );
		
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
