/*	========
 *	sleep.cc
 *	========
 */

// POSIX
#include "unistd.h"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace O = Orion;


int O::Main( int argc, char const *const argv[] )
{
	// Check for correct number of args
	if ( argc != 2 )
	{
		Io::Err << "usage: sleep seconds\n";
		return 1;
	}
	
	unsigned int seconds = std::atoi( argv[ 1 ] );
	
	sleep( seconds );
	
	return 0;
}

