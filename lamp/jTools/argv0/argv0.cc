/*	========
 *	argv0.cc
 *	========
 */

// Standard C
#include <errno.h>

// POSIX
#include <unistd.h>

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace O = Orion;


int O::Main( int argc, char const *const argv[] )
{
	if ( argc < 3 )
	{
		Io::Err << "Usage: argv0 command arg0 [ arg1 ... argn ]\n";
		
		return 1;
	}
	
	int result = execvp( argv[ 1 ], argv + 2 );
	
	if ( result == -1 )
	{
		int error = errno;
		
		Io::Err << argv[ 0 ] << ": execvp() got errno " << error << "\n";
	}
	
	return result ? 1 : 0;
}

