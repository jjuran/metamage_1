/*	===========
 *	readlink.cc
 *	===========
 */

// Standard C
#include <errno.h>
#include <stdio.h>

// POSIX
#include <unistd.h>

// Iota
#include "iota/strings.hh"


#pragma exceptions off


int main( int argc, char const *const argv[] )
{
	// Check for correct number of args
	if ( argc != 2 )
	{
		(void) write( STDERR_FILENO, STR_LEN( "Usage: readlink <file>\n" ) );
		
		return 1;
	}
	
	char buffer[ 4096 ];
	
	ssize_t size = readlink_k( argv[1], buffer, sizeof buffer - 1 );
	
	if ( size < 0 )
	{
		if ( errno != EINVAL )
		{
			fprintf( stderr, "readlink: %s: %s\n", argv[1], strerror( errno ) );
		}
		
		return EXIT_FAILURE;
	}
	
	buffer[ size++ ] = '\n';
	
	(void) write( STDOUT_FILENO, buffer, size );
	
	return EXIT_SUCCESS;
}

