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

// iota
#include "iota/argv.hh"
#include "iota/strings.hh"

// Convergence
#include "fork_and_exit.hh"

// POSeven
#include "POSeven/functions/close.hh"
#include "POSeven/functions/ioctl.hh"
#include "POSeven/functions/open.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace O = Orion;
	
	
	static int usage()
	{
		(void) write( STDERR_FILENO, STR_LEN( "Usage: daemonize command [ arg1 ... argn ]\n" ) );
		
		return 2;
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		bool keep_cwd    = false;
		bool keep_stdin  = false;
		bool keep_stdout = false;
		bool keep_stderr = false;
		
		const char* ctty = NULL;
		
		O::BindOption( "--cwd",    keep_cwd    );
		O::BindOption( "--stdin",  keep_stdin  );
		O::BindOption( "--stdout", keep_stdout );
		O::BindOption( "--stderr", keep_stderr );
		
		O::BindOption( "--ctty", ctty );
		
		O::GetOptions( argc, argv );
		
		char const *const *free_args = O::FreeArguments();
		
		std::size_t n_args = O::FreeArgumentCount();
		
		if ( n_args < 1 )
		{
			return usage();
		}
		
		// Ignore SIGHUP
		signal( SIGHUP, SIG_IGN );
		
		// Ensure we are not a process group leader
		fork_and_exit( 0 );
		
		// Start a new session with no controlling terminal
		setsid();
		
		NN::Owned< p7::fd_t > stdio;
		
		if ( ctty )
		{
			signal( SIGHUP, SIG_DFL );
			
			stdio = p7::open( ctty, p7::o_rdwr );
			
			p7::ioctl( stdio.get(), TIOCSCTTY, NULL );
		}
		else
		{
			// Ensure we can't acquire a controlling terminal by being session leader
			fork_and_exit( 0 );
			
			stdio = p7::open( "/dev/null", p7::o_rdonly );
		}
		
		if ( !keep_cwd )
		{
			chdir( "/" );
		}
		
		if ( !keep_stdin )
		{
			dup2( stdio, STDIN_FILENO  );
		}
		
		if ( !keep_stdout )
		{
			dup2( stdio, STDOUT_FILENO  );
		}
		
		if ( !keep_stderr )
		{
			dup2( stdio, STDERR_FILENO  );
		}
		
		p7::close( stdio );
		
		(void) execvp( *free_args, (char**) free_args );
		
		bool noSuchFile = errno == ENOENT;
		
		std::fprintf( stderr, "%s: %s: %s\n", argv[0], argv[1], std::strerror( errno ) );
		
		return noSuchFile ? 127 : 126;
	}
	
}

