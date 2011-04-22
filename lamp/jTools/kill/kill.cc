/*	=======
 *	kill.cc
 *	=======
 */

// Standard C
#include <signal.h>
#include <stdlib.h>

// Standard C/C++
#include <cctype>

// POSIX
#include <unistd.h>

// iota
#include "iota/strings.hh"

// gear
#include "gear/parse_decimal.hh"

// more-posix
#include "more/perror.hh"

// klibc
#include "klibc/signal_lookup.hh"


int main( int argc, char **argv )
{
	int sig_number = SIGTERM;
	
	char const *const *argp = argv;
	
	if ( argc > 1  &&  argp[ 1 ][ 0 ] == '-' )
	{
		const char *const sig = argp[ 1 ] + 1;
		
		const bool numeric = std::isdigit( *sig );
		
		// FIXME:  Needs error checking instead of silently using 0
		sig_number = numeric ? gear::parse_unsigned_decimal( sig )
		                     : klibc::signal_lookup        ( sig );
		
		++argp;
		--argc;
	}
	
	if ( argc != 2 )
	{
		(void) write( STDERR_FILENO, STR_LEN( "usage: kill [-sig] pid\n" ) );
		
		return 1;
	}
	
	const pid_t pid = gear::parse_decimal( argp[ 1 ] );
	
	const int killed = kill( pid, sig_number );
	
	if ( killed == -1 )
	{
		more::perror( "kill", argp[1] );
		
		return 1;
	}
	
	return 0;
}

