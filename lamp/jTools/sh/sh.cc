/*	=====
 *	sh.cc
 *	=====
 */

// Standard C++
#include <string>
#include <vector>

// Standard C
#include "stdlib.h"

// POSIX
#include "fcntl.h"
#include "sys/stat.h"
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
#include "PositionalParameters.hh"
#include "ReadExecuteLoop.hh"


namespace P7 = POSeven;
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
		if ( GetInteractiveness() )
		{
			Io::Out << ( gLoginShell ? "logout\n" : "exit\n" );
		}
	}
};

int O::Main( int argc, const char *const argv[] )
{
	setenv( "PS1", "$ ", 0 );
	setenv( "PS2", "> ", 0 );
	setenv( "PS4", "+ ", 0 );
	
	std::string command;
	
	bool restricted = false;
	bool interactive = false;
	bool readingFromStdin = false;
	bool verboseInput = false;
	bool verboseExecution = false;
	
	O::BindOption( "-c", command );
	
	O::BindOption( "-r", restricted );
	O::BindOption( "-i", interactive );
	O::BindOption( "-s", readingFromStdin );
	O::BindOption( "-v", verboseInput );
	O::BindOption( "-x", verboseExecution );
	
	try
	{
		O::GetOptions( argc, argv );
	}
	catch ( O::UndefinedOption& )
	{
		Io::Err << "Undefined option\n";
		
		return 1;
	}
	
	const std::vector< const char* >& freeArgs = O::FreeArguments();
	
	gArgZero = argv[ 0 ];
	
	gParameters = &*freeArgs.begin();
	gParameterCount = freeArgs.size();
	
	P7::FileDescriptor fd( P7::kStdIn_FileNo );
	
	if ( gParameterCount > 0 )
	{
		gArgZero = gParameters[ 0 ];
		
		++gParameters;
		--gParameterCount;
		
		fd = P7::Open( gArgZero, O_RDONLY ).Release();
		
		int controlled = fcntl( fd, F_SETFD, FD_CLOEXEC );
		
		SetInteractiveness( interactive );
	}
	else
	{
		// Read from stdin
		gStandardIn = true;
		
		// If first char of arg 0 is a hyphen (e.g. "-sh") it's a login shell
		gLoginShell = argv[ 0 ][ 0 ] == '-';
		
		SetInteractiveness( true );
		
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
	
	if ( !command.empty() )
	{
		// Run a single command
		return ExecuteCmdLine( command );
	}
	
	OnExit onExit;
	
	return ReadExecuteLoop( fd, GetInteractiveness() );
}

