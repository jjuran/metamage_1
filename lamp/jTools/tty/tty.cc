/*	======
 *	tty.cc
 *	======
 */

// POSIX
#include <unistd.h>

// Iota
#include "iota/strings.hh"


#pragma exceptions off


#pragma export on

int main( int argc, char const *const argv[] )
{
	char buffer[ 256 ];
	
	ssize_t size = ttyname_k( STDIN_FILENO, buffer, sizeof buffer );
	
	if ( size < 0 )
	{
		write( STDOUT_FILENO, STR_LEN( "not a tty\n" ) );
	}
	else if ( size + 1 <= sizeof buffer )
	{
		buffer[ size++ ] = '\n';
		
		write( STDOUT_FILENO, buffer, size );
	}
	else
	{
		write( STDERR_FILENO, STR_LEN( "tty: terminal name too long\n" ) );
		
		return 1;
	}
	
	return 0;
}

#pragma export reset

