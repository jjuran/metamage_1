/*	=======
 *	kill.cc
 *	=======
 */

// Standard C++
#include <string>

// POSIX
#include "signal.h"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace O = Orion;


int O::Main( int argc, char const *const argv[] )
{
	int sig = 15;  // SIGTERM
	
	char const *const *argp = argv;
	
	if ( argc > 1  &&  argp[ 1 ][ 0 ] == '-' )
	{
		sig = std::atoi( argp[ 1 ] + 1 );
		
		++argp;
		--argc;
	}
	
	if ( argc != 2 )
	{
		Io::Err << "kill: usage: kill [-sig] pid\n";
		return 1;
	}
	
	int pid = std::atoi( argp[ 1 ] );
	
	int killed = kill( pid, sig );
	
	if ( killed == -1 )
	{
		Io::Err << "kill: no such process\n";
		return 1;
	}
	
	return 0;
}

