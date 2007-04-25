/*	=====
 *	rm.cc
 *	=====
 */

// Standard C/C++
#include <cerrno>

// POSIX
#include <unistd.h>

// Orion
#include "Orion/Main.hh"


namespace O = Orion;


#define STR_LEN( string )  "" string, (sizeof string - 1)


int O::Main( int argc, const char *const argv[] )
{
	// Check for sufficient number of args
	if ( argc < 2 )
	{
		(void) write( STDERR_FILENO, STR_LEN( "rm: missing arguments\n" ) );
		
		return 1;
	}
	
	// Print each file in turn.  Return whether any errors occurred.
	int fail = 0;
	
	for ( int index = 1;  index < argc;  ++index )
	{
		const char* pathname = argv[ index ];
		
		int unlinked = unlink( pathname );
		
		fail += unlinked != 0;
	}
	
	return (fail == 0) ? 0 : 1;
}

