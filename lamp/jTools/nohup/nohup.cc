/*	========
 *	nohup.cc
 *	========
 */

// Standard C
#include <errno.h>
#include <signal.h>

// POSIX
#include <unistd.h>

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace O = Orion;


int O::Main( int argc, char const *const argv[] )
{
	if ( argc < 2 )
	{
		Io::Err << "Usage: nohup command [ arg1 ... argn ]\n";
		
		return 1;
	}
	
	signal( SIGHUP, SIG_IGN );
	
	int result = execvp( argv[ 1 ], argv + 1 );
	
	if ( result == -1 )
	{
		int error = errno;
		
		Io::Err << argv[ 0 ] << ": execvp() got errno " << error << "\n";
	}
	
	return result ? 1 : 0;
}

