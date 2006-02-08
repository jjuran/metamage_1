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
#include <unistd.h>

// Nucleus
#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/FileDescriptor.h"

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

bool gStandardIn  = false;
bool gInteractive = false;
bool gLoginShell  = false;

const char* gArgZero = NULL;
std::size_t gParameterCount = 0;
char const* const* gParameters = NULL;


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

static void WasteTime()
{
#if TARGET_API_MAC_CARBON
	
	::QDFlushPortBuffer( GetQDGlobalsThePort(), NULL );
	
#endif
	
	UInt32 then = TickCount();
	
	while ( TickCount() - then < 60 * 2 )
	{
		// do nothing
	}
}

int O::Main( int argc, const char *const argv[] )
{
	setenv( "PS1", "$ ", 0 );
	setenv( "PS2", "> ", 0 );
	setenv( "PS4", "+ ", 0 );
	
	O::Options options = DefineOptions();
	
	try
	{
		options.GetOptions( argc, argv );
	}
	catch ( O::UndefinedOption& )
	{
		Io::Err << "Undefined option\n";
		WasteTime();
		
		return 1;
	}
	
	gArgZero = argv[ 0 ];
	
	gParameters = options.Begin();
	gParameterCount = options.GetFreeParams().size();
	
	P7::FileDescriptor fd = Io::in;
	
	if ( gParameterCount > 0 )
	{
		gArgZero = gParameters[ 0 ];
		
		++gParameters;
		--gParameterCount;
		
		fd = open( gArgZero, O_RDONLY );
		
		gInteractive = options.GetFlag( optInteractive );
	}
	else
	{
		// Read from stdin
		gStandardIn = true;
		
		// If first char of arg 0 is a hyphen (e.g. "-sh") it's a login shell
		gLoginShell = argv[ 0 ][ 0 ] == '-';
		
		gInteractive = true;
		
		if ( gLoginShell )
		{
			P7::FileDescriptor profile = open( "/etc/profile", O_RDONLY );
			
			if ( profile != -1 )
			{
				(void)close( profile );
				ExecuteCmdLine( ". /etc/profile" );
			}
		}
	}
	
	std::string command = options.GetString( optCommand );
	
	if ( !command.empty() )
	{
		// Run a single command
		return ExecuteCmdLine( command );
	}
	
	OnExit onExit;
	
	return ReadExecuteLoop( fd, gInteractive );
}

