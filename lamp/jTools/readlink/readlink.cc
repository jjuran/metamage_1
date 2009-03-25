/*	===========
 *	readlink.cc
 *	===========
 */

// Standard C++
#include <algorithm>

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
	
	int exit_status = EXIT_SUCCESS;
	
	char buffer[ 1024 ];
	
	ssize_t size = readlink_k( argv[1], buffer, sizeof buffer );
	
	if ( size < 0 )
	{
		return 1;  // No error output
	}
	else if ( size + 1 > sizeof buffer )
	{
		size = sizeof buffer;
		
		const char* tail = "...\n";
		
		std::copy_backward( tail, tail + STRLEN( "...\n" ), buffer + size );
		
		exit_status = EXIT_FAILURE;
	}
	else
	{
		buffer[ size++ ] = '\n';
	}
	
	(void) write( STDOUT_FILENO, buffer, size );
	
	return exit_status;
}

