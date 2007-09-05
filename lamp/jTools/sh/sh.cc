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
#include <unistd.h>

// Nucleus
#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/Open.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"

// sh
#include "Builtins.hh"
#include "Execution.hh"
#include "Options.hh"
#include "PositionalParameters.hh"
#include "ReadExecuteLoop.hh"


namespace p7 = poseven;
namespace O = Orion;


bool gStandardIn  = false;
bool gLoginShell  = false;

const char* gArgZero = NULL;
std::size_t gParameterCount = 0;
char const* const* gParameters = NULL;


struct OnExit
{
	~OnExit()
	{
		if ( GetOption( kOptionInteractive ) )
		{
			Io::Out << ( gLoginShell ? "logout\n" : "exit\n" );
		}
	}
};

int O::Main( int argc, argv_t argv )
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
	
	try
	{
		O::GetOptions( argc, argv );
	}
	catch ( O::UndefinedOption& )
	{
		Io::Err << "Undefined option\n";
		
		return 1;
	}
	
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
		signal( SIGCHLD, SIG_IGN );
		
		pid_t pid = getpid();
		
		if ( shell_pgid != pid )
		{
			setpgid( pid, pid );
		}
		
		// set fg pg
		tcsetpgrp( STDIN_FILENO, shell_pgid );
		
		// save terminal attrs
	}
	
	if ( *freeArgs != NULL )
	{
		gArgZero = gParameters[ 0 ];
		
		++gParameters;
		--gParameterCount;
		
		input = p7::open( gArgZero, O_RDONLY ).Release();
		
		int controlled = fcntl( input, F_SETFD, FD_CLOEXEC );
	}
	else
	{
		// Read from stdin
		gStandardIn = true;
		
		if ( gLoginShell )
		{
			struct ::stat sb;
			
			int statted = stat( "/etc/profile", &sb );
			
			if ( statted != -1 )
			{
				ExecuteCmdLine( ". /etc/profile" );
			}
		}
	}
	
	if ( command != NULL )
	{
		// Run a single command
		return ExecuteCmdLine( command );
	}
	
	OnExit onExit;
	
	return ReadExecuteLoop( input, GetOption( kOptionInteractive ) );
}

