/*	=====
 *	sh.cc
 *	=====
 */

// Standard C++
#include <string>
#include <vector>

// Standard C
#include <signal.h>
#include <stdlib.h>

// POSIX
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

// Nucleus
#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/Open.hh"
#include "POSeven/functions/fcntl.hh"
#include "POSeven/functions/signal.hh"
#include "POSeven/functions/stat.hh"
#include "POSeven/functions/wait.hh"

// Iota
#include "iota/strings.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"

// sh
#include "Builtins.hh"
#include "Execution.hh"
#include "Options.hh"
#include "PositionalParameters.hh"
#include "ReadExecuteLoop.hh"


namespace tool
{
	
	namespace p7 = poseven;
	namespace O = Orion;
	
	
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
		
		p7::signal( p7::sigint,  p7::sig_ign );
		p7::signal( p7::sigquit, p7::sig_ign );
		p7::signal( p7::sigtstp, p7::sig_ign );
		p7::signal( p7::sigttin, p7::sig_ign );
		p7::signal( p7::sigttou, p7::sig_ign );
		
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
		
		O::BindOption( "-c", command );
		
		O::BindOption( "-l", gLoginShell );
		O::BindOption( "-i", interactive );
		O::BindOption( "-m", monitor );
		O::BindOption( "-r", restricted );
		O::BindOption( "-s", readingFromStdin );
		O::BindOption( "-v", verboseInput );
		O::BindOption( "-x", verboseExecution );
		
		O::AliasOption( "-l", "--login"   );
		O::AliasOption( "-v", "--verbose" );
		
		O::GetOptions( argc, argv );
		
		gArgZero = argv[ 0 ];
		
		char const *const *freeArgs = O::FreeArguments();
		
		// If first char of arg 0 is a hyphen (e.g. "-sh") it's a login shell
		gLoginShell = gLoginShell  ||  argv[ 0 ][ 0 ] == '-';
		
		interactive = interactive  ||  *freeArgs == NULL && command == NULL && isatty( 0 ) && isatty( 2 );
		
		monitor = monitor || interactive;
		
		SetOption( kOptionInteractive, interactive );
		SetOption( kOptionMonitor,     monitor     );
		
		gParameters = freeArgs;
		gParameterCount = O::FreeArgumentCount();
		
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
				
				input = p7::open( gArgZero, flags ).Release();
				
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
			return Nucleus::Convert< p7::exit_t >( ExecuteCmdLine( command ) );
		}
		
		OnExit onExit;
		
		p7::wait_t status = ReadExecuteLoop( input,
		                                     GetOption( kOptionInteractive ) );
		
		return Nucleus::Convert< p7::exit_t >( status );
	}
	
}

