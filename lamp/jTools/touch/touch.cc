/*	========
 *	touch.cc
 *	========
 */

// Standard C/C++
#include <cstdio>

// POSIX
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

// Orion
#include "Orion/Main.hh"


namespace O = Orion;


int O::Main( int argc, char const* const argv[] )
{
	// Check for sufficient number of args
	if ( argc < 2 )
	{
		std::fprintf( stderr, "touch: missing arguments\n" );
		
		return 1;
	}
	
	// Try to touch each item.  Return whether any errors occurred.
	int fail = 0;
	
	for ( std::size_t index = 1;  index < argc;  ++index )
	{
		const char* pathname = argv[ index ];
		
		int fd = open( pathname, O_WRONLY | O_CREAT, 0600 );
		
		if ( fd == -1 )
		{
			std::fprintf( stderr, "touch: %s: %s\n", pathname, strerror( errno ) );
			
			++fail;
		}
		else
		{
			close( fd );
		}
	}
	
	return (fail == 0) ? 0 : 1;
}

