/*
	daemonize.cc
	------------
*/

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

// POSIX
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

// command
#include "command/get_option.hh"

// must
#include "must/chdir.h"
#include "must/write.h"

// iota
#include "iota/strings.hh"

// Relix
#include "relix/fork_and_exit.h"

// poseven
#include "poseven/functions/perror.hh"


using namespace command::constants;

enum
{
	Option_last_byte = 255,
	
	Option_keep_cwd,
	Option_keep_stdin,
	Option_keep_stdout,
	Option_keep_stderr,
	Option_new_ctty,
};

static command::option options[] =
{
	{ "cwd",    Option_keep_cwd    },
	{ "stdin",  Option_keep_stdin  },
	{ "stdout", Option_keep_stdout },
	{ "stderr", Option_keep_stderr },
	
	{ "ctty",   Option_new_ctty, Param_required },
	
	{ NULL }
};

static bool keep_cwd    = false;
static bool keep_stdin  = false;
static bool keep_stdout = false;
static bool keep_stderr = false;

static const char* ctty = NULL;


static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_keep_cwd:
				keep_cwd = true;
				break;
			
			case Option_keep_stdin:
				keep_stdin = true;
				break;
			
			case Option_keep_stdout:
				keep_stdout = true;
				break;
			
			case Option_keep_stderr:
				keep_stderr = true;
				break;
			
			case Option_new_ctty:
				ctty = command::global_result.param;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}


namespace p7 = poseven;


static int usage()
{
	must_write( STDERR_FILENO, STR_LEN( "Usage: daemonize command [ arg1 ... argn ]\n" ) );
	
	return 2;
}

int main( int argc, char** argv )
{
	char *const *args = get_options( argv );
	
	const int argn = argc - (args - argv);
	
	if ( argn < 1 )
	{
		return usage();
	}
	
	struct sigaction action = { 0 };
	
	action.sa_handler = SIG_IGN;
	
	// Ignore SIGHUP
	sigaction( SIGHUP, &action, NULL );
	
	// Ensure we are not a process group leader
	fork_and_exit( 0 );
	
	// Start a new session with no controlling terminal
	setsid();
	
	int stdio;
	
	if ( ctty )
	{
		action.sa_handler = SIG_DFL;
		
		sigaction( SIGHUP, &action, NULL );
		
		stdio = open( ctty, O_RDWR );
		
	#ifdef TIOCSCTTY
		
		if ( stdio >= 0 )
		{
			ioctl( stdio, TIOCSCTTY, NULL );
		}
		
	#endif
	}
	else
	{
		// Ensure we can't acquire a controlling terminal by being session leader
		fork_and_exit( 0 );
		
		stdio = open( "/dev/null", O_RDWR );
	}
	
	if ( stdio < 0 )
	{
		p7::perror( argv[0], "/dev/null" );
		return 1;
	}
	
	if ( !keep_cwd )
	{
		must_chdir( "/" );
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
	
	close( stdio );
	
	(void) execvp( *args, (char**) args );
	
	bool noSuchFile = errno == ENOENT;
	
	p7::perror( argv[0], argv[1] );
	
	return noSuchFile ? 127 : 126;
}
