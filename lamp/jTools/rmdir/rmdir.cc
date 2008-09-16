/*	========
 *	rmdir.cc
 *	========
 */

// Standard C/C++
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// POSIX
#include <unistd.h>

// Orion
#include "Orion/Main.hh"


#define STR_LEN( string )  "" string, (sizeof string - 1)


namespace tool
{
	
	int Main( int argc, iota::argv_t argv )
	{
		// Check for sufficient number of args
		if ( argc < 2 )
		{
			(void) write( STDERR_FILENO, STR_LEN( "rmdir: missing arguments\n" ) );
			
			return 1;
		}
		
		int exit_status = EXIT_SUCCESS;
		
		for ( int index = 1;  index < argc;  ++index )
		{
			const char* pathname = argv[ index ];
			
			int removed = rmdir( pathname );
			
			if ( removed == -1 )
			{
				exit_status = EXIT_FAILURE;
				
				std::fprintf( stderr, "rmdir: %s: %s\n", pathname, std::strerror( errno ) );
			}
		}
		
		return exit_status;
	}
	
}

namespace Orion
{
	
	int Main( int argc, iota::argv_t argv )
	{
		return tool::Main( argc, argv );
	}
	
}

