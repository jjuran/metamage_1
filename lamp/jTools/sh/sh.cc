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

// Io
#include "Io/MakeHandle.hh"
#include "Io/TextInput.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"

// sh
#include "Builtins.hh"
#include "Execution.hh"
#include "ReadExecuteLoop.hh"


namespace N = Nitrogen;
namespace O = Orion;


enum
{
	optCommand, 
	optRestricted, 
	optInteractive, 
	optStdin, 
	optVerboseInput, 
	optVerboseExecution
};

static O::Options DefineOptions()
{
	O::Options options;
	
	options.DefineSetString( "-c", optCommand );
	
	options.DefineSetFlag( "-r", optRestricted       );
	options.DefineSetFlag( "-i", optInteractive      );
	options.DefineSetFlag( "-s", optStdin            );
	options.DefineSetFlag( "-v", optVerboseInput     );
	options.DefineSetFlag( "-x", optVerboseExecution );
	
	return options;
}

bool gInteractive = false;
bool gLoginShell  = false;


struct OnExit
{
	~OnExit()
	{
		if ( gInteractive )
		{
			Io::Out << ( gLoginShell ? "logout\n" : "exit\n" );
		}
	}
};

int O::Main( int argc, const char *const argv[] )
{
	O::Options options = DefineOptions();
	
	options.GetOptions( argc, argv );
	
	const std::vector< const char* >& params = options.GetFreeParams();
	
	std::string command = options.GetString( optCommand );
	
	if ( !command.empty() )
	{
		// Run a single command
		return ExecuteCmdLine( command, params );
	}
	
	int fd = Io::in;
	
	if ( params.empty() || options.GetFlag( optStdin ) )
	{
		// Read from stdin
		
		// First char of arg 0 is a hyphen, as in "-sh"
		gLoginShell = argv[ 0 ][ 0 ] == '-';
		
		gInteractive = params.empty() || options.GetFlag( optInteractive );
		
		//if ( gLoginShell )
		{
			setenv( "PS1", "$ ", 0 );
			setenv( "PS2", "> ", 0 );
			setenv( "PS4", "+ ", 0 );
		}
	}
	else
	{
		// Otherwise, run a shell script
		fd = open( params[ 0 ], 0 );
	}
	
	Io::Handle in = Io::MakeHandleFromCast< Io::FD_Details, Io::FD >( Io::FD( fd ) );
	
	OnExit onExit;
	
	return ReadExecuteLoop( in, params );
}

