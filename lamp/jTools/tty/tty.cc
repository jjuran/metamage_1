/*	======
 *	tty.cc
 *	======
 */

// Standard C
#include <errno.h>

// POSIX
#include <unistd.h>

// Iota
#include "iota/strings.hh"


#pragma exceptions off


int main( int argc, char const *const argv[] )
{
	if ( char* buffer = ttyname( STDIN_FILENO ) )
	{
		size_t size = strlen( buffer );
		
		buffer[ size++ ] = '\n';
		
		write( STDOUT_FILENO, buffer, size );
	}
	else if ( errno == ENOTTY  ||  errno == ENODEV )
	{
		write( STDOUT_FILENO, STR_LEN( "not a tty\n" ) );
	}
	else
	{
		const char* error = strerror( errno );
		
		write( STDERR_FILENO, STR_LEN( "tty: " ) );
		
		write( STDERR_FILENO, error, strlen( error ) );
		
		write( STDERR_FILENO, STR_LEN( "\n" ) );
		
		return 1;
	}
	
	return 0;
}

