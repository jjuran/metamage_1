/*	=====
 *	sh.cc
 *	=====
 */

// Standard C++
#include <string>
#include <vector>

// Standard C
#include <stdlib.h>

// POSIX
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

// poseven
#include "poseven/functions/fcntl.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/sigaction.hh"
#include "poseven/functions/stat.hh"
#include "poseven/functions/wait.hh"

// Iota
#include "iota/strings.hh"

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"

// sh
#include "Builtins.hh"
#include "Execution.hh"
#include "Options.hh"
#include "PositionalParameters.hh"
#include "ReadExecuteLoop.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	namespace o = orion;
	
	
	const char*         gArgZero        = NULL;
	std::size_t         gParameterCount = 0;
	char const* const*  gParameters     = NULL;
	
	
	static bool gLoginShell = false;
	
	
	static void start_monitoring()
	{
		pid_t shell_pgid = getpgrp();
		
		// stop until we're fg
		while ( tcgetpgrp( STDIN_FILENO ) != shell_pgid )
		{
			kill( -shell_pgid, SIGTTIN );
		}
		
		p7::sigaction( p7::sigint,  p7::sig_ign );
		p7::sigaction( p7::sigquit, p7::sig_ign );
		p7::sigaction( p7::sigtstp, p7::sig_ign );
		p7::sigaction( p7::sigttin, p7::sig_ign );
		p7::sigaction( p7::sigttou, p7::sig_ign );
		
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
					(void) write( STDOUT_FILENO, STR_LEN( "logout\n" ) );
				}
				else
				{
					(void) write( STDOUT_FILENO, STR_LEN( "exit\n" ) );
				}
			}
		}
	};
	
	int Main( int argc, iota::argv_t argv )
	{
		setenv( "PS1", "$ ", 0 );
		setenv( "PS2", "> ", 0 );
		setenv( "PS4", "+ ", 0 );
		
		const char* command = NULL;
		
		bool interactive = false;
		bool monitor = false;
		bool restricted = false;
		bool readingFromStdin = false;
		bool verboseInput = false;
		bool verboseExecution = false;
		
		o::bind_option_to_variable( "-c", command );
		
		o::bind_option_to_variable( "-l", gLoginShell );
		o::bind_option_to_variable( "-i", interactive );
		o::bind_option_to_variable( "-m", monitor );
		o::bind_option_to_variable( "-r", restricted );
		o::bind_option_to_variable( "-s", readingFromStdin );
		o::bind_option_to_variable( "-v", verboseInput );
		o::bind_option_to_variable( "-x", verboseExecution );
		
		o::alias_option( "-l", "--login"   );
		o::alias_option( "-v", "--verbose" );
		
		o::get_options( argc, argv );
		
		gArgZero = argv[ 0 ];
		
		char const *const *freeArgs = o::free_arguments();
		
		const size_t n_args = o::free_argument_count();
		
		// If first char of arg 0 is a hyphen (e.g. "-sh") it's a login shell
		gLoginShell = gLoginShell  ||  argv[ 0 ][ 0 ] == '-';
		
		interactive = interactive  ||  *freeArgs == NULL && command == NULL && isatty( 0 ) && isatty( 2 );
		
		monitor = monitor || interactive;
		
		SetOption( kOptionInteractive, interactive );
		SetOption( kOptionMonitor,     monitor     );
		
		gParameters = freeArgs;
		gParameterCount = n_args;
		
		p7::fd_t input( p7::stdin_fileno );
		
		if ( monitor )
		{
			start_monitoring();
		}
		
		if ( *freeArgs != NULL )
		{
			gArgZero = gParameters[ 0 ];
			
			++gParameters;
			--gParameterCount;
			
			if ( command == NULL )
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
		else
		{
			// Read from stdin
			
			if ( gLoginShell && io::item_exists( "/etc/profile" ) )
			{
				ExecuteCmdLine( ". /etc/profile" );
			}
		}
		
		if ( command != NULL )
		{
			// Run a single command
			return n::convert< p7::exit_t >( ExecuteCmdLine( command ) );
		}
		
		OnExit onExit;
		
		p7::wait_t status = ReadExecuteLoop( input,
		                                     GetOption( kOptionInteractive ) );
		
		return n::convert< p7::exit_t >( status );
	}
	
}

