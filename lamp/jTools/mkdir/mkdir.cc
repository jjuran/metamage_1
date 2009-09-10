/*	========
 *	mkdir.cc
 *	========
 */

// Standard C/C++
#include <cstdio>
#include <cstring>

// Standard C
#include <errno.h>

// POSIX
#include <sys/stat.h>


#pragma exceptions off


int main( int argc, char *const *argv )
{
	// Check for sufficient number of args
	if ( argc < 2 )
	{
		std::fprintf( stderr, "mkdir: missing arguments\n" );
		return 1;
	}
	
	// Try to make each directory.  Return whether any errors occurred.
	int fail = 0;
	
	for ( int index = 1;  index < argc;  ++index )
	{
		int result = mkdir( argv[ index ], 0700 );
		
		if ( result == -1 )
		{
			std::fprintf( stderr,
			              "mkdir: %s: %s\n",
			                      argv[ index ],
			                          std::strerror( errno ) );
			fail++;
		}
	}
	
	return (fail == 0) ? 0 : 1;
}

