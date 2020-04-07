/*	======
 *	pwd.cc
 *	======
 */

// Standard C
#include <errno.h>
#include <string.h>

// POSIX
#include <unistd.h>


#pragma exceptions off


#define STR_LEN( s )  "" s, (sizeof s - 1)


int main( int argc, char const *const argv[] )
{
	char buffer[ 4096 ];
	
	ssize_t size = _getcwd( buffer, sizeof buffer - 1 );
	
	if ( size < 0 )
	{
		const char* error = strerror( errno );
		
		write( STDERR_FILENO, STR_LEN( "pwd: " ) );
		
		write( STDERR_FILENO, error, strlen( error ) );
		
		write( STDERR_FILENO, STR_LEN( "\n" ) );
		
		return 1;
	}
	
	buffer[ size++ ] = '\n';
	
	write( STDOUT_FILENO, buffer, size );
	
	return 0;
}
