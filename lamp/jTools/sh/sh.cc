/*	=====
 *	sh.cc
 *	=====
 */

// Standard C
#include <signal.h>
#include <stdlib.h>

// POSIX
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

// must
#include "must/write.h"

// command
#include "command/get_option.hh"

// poseven
#include "poseven/functions/fcntl.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/stat.hh"
#include "poseven/functions/wait.hh"

// Iota
#include "iota/strings.hh"

// Orion
#include "Orion/Main.hh"

// sh
#include "Builtins.hh"
#include "Execution.hh"
#include "Options.hh"
#include "PositionalParameters.hh"
#include "ReadExecuteLoop.hh"


using namespace command::constants;

enum
{
	Option_command     = 'c',
	Option_interactive = 'i',
	Option_login_shell = 'l',
	Option_monitor     = 'm',
	Option_restricted  = 'r',
	Option_read_stdin  = 's',
	Option_verbose     = 'v',
	Option_verbose_x   = 'x',
};

static command::option options[] =
{
	{ "", Option_command, Param_required },
	
	{ "", Option_interactive },
	{ "", Option_monitor     },
	{ "", Option_restricted  },
	{ "", Option_read_stdin  },
	{ "", Option_verbose_x   },
	
	{ "login",   Option_login_shell },
	{ "verbose", Option_verbose     },
	{ NULL }
};

static bool gLoginShell = false;

static const char* the_command = NULL;

static bool interactive = false;
static bool monitor = false;
static bool restricted = false;
static bool readingFromStdin = false;
static bool verboseInput = false;
static bool verboseExecution = false;

static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_command:
				the_command = command::global_result.param;
				break;
			
			case Option_login_shell:
				gLoginShell = true;
				break;
			
			case Option_interactive:
				interactive = true;
				break;
			
			case Option_monitor:
				monitor = true;
				break;
			
			case Option_restricted:
				restricted = true;
				break;
			
			case Option_read_stdin:
				readingFromStdin = true;
				break;
			
			case Option_verbose:
				verboseInput = true;
				break;
			
			case Option_verbose_x:
				verboseExecution = true;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	const char*         gArgZero        = NULL;
	unsigned            gParameterCount = 0;
	char const* const*  gParameters     = NULL;
	
	
	static void start_monitoring()
	{
		pid_t shell_pgid = getpgrp();
		
		// stop until we're fg
		while ( tcgetpgrp( STDIN_FILENO ) != shell_pgid )
		{
			kill( -shell_pgid, SIGTTIN );
		}
		
		signal( SIGINT,  SIG_IGN );
		signal( SIGQUIT, SIG_IGN );
		signal( SIGTSTP, SIG_IGN );
		signal( SIGTTIN, SIG_IGN );
		signal( SIGTTOU, SIG_IGN );
		
		pid_t pid = getpid();
		
		if ( shell_pgid != pid )
		{
			setpgid( pid, pid );
		}
		
		// set fg pg
		tcsetpgrp( STDIN_FILENO, pid );
		
		// save terminal attrs
	}
	
	struct OnExit
	{
		~OnExit()
		{
			if ( GetOption( kOptionInteractive ) )
			{
				if ( gLoginShell )
				{
					must_write( STDOUT_FILENO, STR_LEN( "logout\n" ) );
				}
				else
				{
					must_write( STDOUT_FILENO, STR_LEN( "exit\n" ) );
				}
			}
		}
	};
	
	int Main( int argc, char** argv )
	{
		setenv( "PS1", "$ ", 0 );
		setenv( "PS2", "> ", 0 );
		setenv( "PS4", "+ ", 0 );
		
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		gArgZero = argv[ 0 ];
		
		// If first char of arg 0 is a hyphen (e.g. "-sh") it's a login shell
		gLoginShell = gLoginShell  ||  argv[ 0 ][ 0 ] == '-';
		
		interactive = interactive  ||  (*args == NULL && the_command == NULL && isatty( 0 ) && isatty( 2 ));
		
		monitor = monitor || interactive;
		
		SetOption( kOptionInteractive, interactive );
		SetOption( kOptionMonitor,     monitor     );
		
		gParameters = args;
		gParameterCount = argn;
		
		p7::fd_t input( p7::stdin_fileno );
		
		if ( monitor )
		{
			start_monitoring();
		}
		
		// Unset stale PWD and OLDPWD, cd . to prime PWD, export unset OLDPWD
		ExecuteCmdLine( "unset PWD; unset OLDPWD; cd .; export OLDPWD" );
		
		if ( *args != NULL )
		{
			gArgZero = gParameters[ 0 ];
			
			++gParameters;
			--gParameterCount;
			
			if ( the_command == NULL )
			{
			#ifdef O_CLOEXEC
				
				const p7::open_flags_t flags = p7::o_rdonly | p7::o_cloexec;
				
			#else
				
				const p7::open_flags_t flags = p7::o_rdonly;
				
			#endif
				
				input = p7::open( gArgZero, flags ).release();
				
			#ifndef O_CLOEXEC
				
				p7::fcntl< p7::f_setfd >( input, p7::fd_cloexec );
				
			#endif
			}
		}
		else if ( gLoginShell )
		{
			// Read from stdin
			
			struct stat stat_buffer;
			
			if ( p7::stat( "/etc/profile", stat_buffer ) )
			{
				ExecuteCmdLine( ". /etc/profile" );
			}
			
			/*
				FIXME:  We expect `login` to chdir to $HOME, but there are
				other ways to request a login shell, and .profile should be
				read from $HOME, not the cwd.  But this works for `login`.
			*/
			
			if ( p7::stat( "./.relix_profile", stat_buffer ) )
			{
				ExecuteCmdLine( ". ./.relix_profile" );
			}
		}
		
		if ( the_command != NULL )
		{
			// Run a single command
			return n::convert< p7::exit_t >( ExecuteCmdLine( the_command ) );
		}
		
		OnExit onExit;
		
		p7::wait_t status = ReadExecuteLoop( input,
		                                     GetOption( kOptionInteractive ) );
		
		return n::convert< p7::exit_t >( status );
	}
	
}
