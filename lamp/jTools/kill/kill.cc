/*	=======
 *	kill.cc
 *	=======
 */

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

// Standard C/C++
#include <cctype>
#include <string>

// POSIX
#include <unistd.h>

// Orion
#include "Orion/Main.hh"
#include "Orion/SignalLookup.hh"


namespace tool
{
	
	int Main( int argc, iota::argv_t argv )
	{
		int sig_number = SIGTERM;
		
		char const *const *argp = argv;
		
		if ( argc > 1  &&  argp[ 1 ][ 0 ] == '-' )
		{
			const char* sig = argp[ 1 ] + 1;
			
			bool numeric = std::isdigit( *sig );
			
			// FIXME:  Needs error checking instead of silently using 0
			sig_number = numeric ? std::atoi( sig ) : Orion::SignalLookup( sig );
			
			++argp;
			--argc;
		}
		
		if ( argc != 2 )
		{
			const char usage[] = "kill: usage: kill [-sig] pid\n";
			
			(void) write( STDERR_FILENO, usage, sizeof usage - 1 );
			
			return 1;
		}
		
		int pid = std::atoi( argp[ 1 ] );
		
		int killed = kill( pid, sig_number );
		
		if ( killed == -1 )
		{
			std::fprintf( stderr, "kill: %s: %s\n", argp[1], std::strerror( errno ) );
			
			return 1;
		}
		
		return 0;
	}
	
}

namespace Orion
{
	
	int Main( int argc, iota::argv_t argv )
	{
		return tool::Main( argc, argv );
	}
	
}

