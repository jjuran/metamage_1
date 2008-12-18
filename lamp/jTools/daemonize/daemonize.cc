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

// Convergence
#include "fork_and_exit.hh"


static int usage()
{
	(void) write( STDERR_FILENO, STR_LEN( "Usage: daemonize command [ arg1 ... argn ]\n" ) );
	
	return 2;
}

int main( int argc, iota::argv_t argv )
{
	if ( argc < 2 )
	{
		return usage();
	}
	
	char const *const *args = argv + 1;
	
	bool keep_cwd = false;
	
	if ( std::strcmp( argv[1], "--cwd" ) == 0 )
	{
		if ( argc < 3 )
		{
			return usage();
		}
		
		keep_cwd = true;
		
		++args;
	}
	
	// Ignore SIGHUP
	signal( SIGHUP, SIG_IGN );
	
	// Ensure we are not a process group leader
	fork_and_exit( 0 );
	
	// Start a new session with no controlling terminal
	setsid();
	
	// Ensure we can't acquire a controlling terminal by being session leader
	fork_and_exit( 0 );
	
	if ( !keep_cwd )
	{
		chdir( "/" );
	}
	
	int devnull = open( "/dev/null", O_RDWR, 0 );
	
	dup2( devnull, STDIN_FILENO  );
	dup2( devnull, STDOUT_FILENO );
	dup2( devnull, STDERR_FILENO );
	
	close( devnull );
	
	(void) execvp( *args, (char**) args );
	
	bool noSuchFile = errno == ENOENT;
	
	std::fprintf( stderr, "%s: %s: %s\n", argv[0], argv[1], std::strerror( errno ) );
	
	return noSuchFile ? 127 : 126;
}

