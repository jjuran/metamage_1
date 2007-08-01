/*	===========
 *	readlink.cc
 *	===========
 */

// POSIX
#include <unistd.h>

// Nucleus
#include "Nucleus/NAssert.h"


#pragma exceptions off


#pragma export on

int main( int argc, char const *const argv[] )
{
	// Check for correct number of args
	if ( argc != 2 )
	{
		const char usage[] = "usage: readlink <file>\n";
		
		(void) write( STDERR_FILENO, usage, sizeof usage - 1 );
		
		return 1;
	}
	
	const std::size_t path_len = 1024;
	
	char target_path[ path_len + 1];  // leave room for "\n"
	
	int result = readlink( argv[1], target_path, path_len );
	
	if ( result == -1 )
	{
		return 1;  // No error output
	}
	
	ASSERT( result > 0 );
	
	target_path[ result++ ] = '\n';
	
	(void) write( STDOUT_FILENO, target_path, result );
	
	return 0;
}

#pragma export reset

