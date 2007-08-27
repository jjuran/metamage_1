/*	============
 *	daemonize.cc
 *	============
 */

// Standard C/C++
#include <cstdio>
#include <cstring>

// Standard C
#include <errno.h>
#include <signal.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>

// iota
#include "iota/argv.hh"
#include "iota/strings.hh"

// kernelramp
#include "fork_and_exit.hh"


#pragma export on

int main( int argc, iota::argv_t argv )
{
	if ( argc < 2 )
	{
		(void) write( STDERR_FILENO, STR_LEN( "Usage: daemonize command [ arg1 ... argn ]\n" ) );
		
		return 1;
	}
	
	// Ensure we are not a process group leader
	fork_and_exit( 0 );
	
	// Start a new session with no controlling terminal
	setsid();
	
	// Ignore SIGHUP
	signal( SIGHUP, SIG_IGN );
	
	// Ensure we can't acquire a controlling terminal by being session leader
	fork_and_exit( 0 );
	
	chdir( "/" );
	
	int devnull = open( "/dev/null", O_RDWR, 0 );
	
	dup2( devnull, STDIN_FILENO  );
	dup2( devnull, STDOUT_FILENO );
	dup2( devnull, STDERR_FILENO );
	
	close( devnull );
	
	(void) execvp( argv[ 1 ], argv + 1 );
	
	bool noSuchFile = errno == ENOENT;
	
	std::fprintf( stderr, "%s: %s: %s\n", argv[0], argv[1], std::strerror( errno ) );
	
	return noSuchFile ? 127 : 126;
}

#pragma export reset

