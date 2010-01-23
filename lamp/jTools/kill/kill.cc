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
#include <cstdio>
#include <cstdlib>
#include <cstring>

// POSIX
#include <unistd.h>

// iota
#include "iota/strings.hh"

// klibc
#include "klibc/signal_lookup.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	int Main( int argc, iota::argv_t argv )
	{
		int sig_number = SIGTERM;
		
		char const *const *argp = argv;
		
		if ( argc > 1  &&  argp[ 1 ][ 0 ] == '-' )
		{
			const char *const sig = argp[ 1 ] + 1;
			
			const bool numeric = std::isdigit( *sig );
			
			// FIXME:  Needs error checking instead of silently using 0
			sig_number = numeric ? std::atoi( sig ) : klibc::signal_lookup( sig );
			
			++argp;
			--argc;
		}
		
		if ( argc != 2 )
		{
			(void) write( STDERR_FILENO, STR_LEN( "usage: kill [-sig] pid\n" ) );
			
			return 1;
		}
		
		const pid_t pid = std::atoi( argp[ 1 ] );
		
		const int killed = kill( pid, sig_number );
		
		if ( killed == -1 )
		{
			std::fprintf( stderr, "kill: %s: %s\n", argp[1], std::strerror( errno ) );
			
			return 1;
		}
		
		return 0;
	}
	
}

