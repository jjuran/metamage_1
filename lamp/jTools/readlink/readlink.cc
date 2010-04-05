/*	===========
 *	readlink.cc
 *	===========
 */

// Standard C
#include <errno.h>

// POSIX
#include <unistd.h>

// Iota
#include "iota/strings.hh"

// more-posix
#include "more/perror.hh"


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
	
	ssize_t size = _readlink( argv[1], buffer, sizeof buffer - 1 );
	
	if ( size < 0 )
	{
		if ( errno != EINVAL )
		{
			more::perror( "readlink", argv[1] );
		}
		
		return EXIT_FAILURE;
	}
	
	buffer[ size++ ] = '\n';
	
	(void) write( STDOUT_FILENO, buffer, size );
	
	return EXIT_SUCCESS;
}

