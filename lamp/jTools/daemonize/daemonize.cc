/*	============
 *	daemonize.cc
 *	============
 */

// Standard C/C++
#include <cstring>

// Standard C
#include <errno.h>
#include <signal.h>

// iota
#include "iota/argv.hh"
#include "iota/strings.hh"

// Convergence
#include "fork_and_exit.hh"

// poseven
#include "poseven/functions/close.hh"
#include "poseven/functions/ioctl.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/perror.hh"

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	namespace o = orion;
	
	
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
		
		o::bind_option_to_variable( "--cwd",    keep_cwd    );
		o::bind_option_to_variable( "--stdin",  keep_stdin  );
		o::bind_option_to_variable( "--stdout", keep_stdout );
		o::bind_option_to_variable( "--stderr", keep_stderr );
		
		o::bind_option_to_variable( "--ctty", ctty );
		
		o::get_options( argc, argv );
		
		char const *const *free_args = o::free_arguments();
		
		std::size_t n_args = o::free_argument_count();
		
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
		
		n::owned< p7::fd_t > stdio;
		
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
		
		p7::perror( argv[0], argv[1] );
		
		return noSuchFile ? 127 : 126;
	}
	
}

