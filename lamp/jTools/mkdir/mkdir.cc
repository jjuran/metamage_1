/*	========
 *	mkdir.cc
 *	========
 */

// Universal Interfaces
#include <MacTypes.h>

#include <cstdio>

// Nitrogen
#include "Nitrogen/OSStatus.h"

// Orion
#include "Orion/Main.hh"
#include "SystemCalls.hh"


namespace N = Nitrogen;
namespace O = Orion;


int O::Main( int argc, const char *const argv[] )
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
		try
		{
			N::FSpDirCreate( Path2FSS( argv[ index ] ) );
		}
		catch ( N::OSStatus& err )
		{
			std::fprintf( stderr,
			              "mkdir: %s: %s\n",
			              argv[ index ],
			              err == dupFNErr ? "already exists"
			                              : "nonexistent directory in pathname" );
			fail++;
		}
	}
	
	return (fail == 0) ? 0 : 1;
}

