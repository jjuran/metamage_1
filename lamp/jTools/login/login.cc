/*	========
 *	login.cc
 *	========
 */

// Standard C
#include "stdlib.h"

// POSIX
#include "unistd.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Orion
#include "Orion/Main.hh"


namespace N = Nitrogen;
namespace O = Orion;


static void SetVariables()
{
	setenv( "HOSTTYPE", "powerpc"            , 0 );
	setenv( "OSTYPE",                  "lamp", 0 );
	setenv( "MACHTYPE", "powerpc-jtools-lamp", 0 );
	
	FSSpec genie = N::GetProcessAppSpec( N::CurrentProcess() );
	N::FSDirSpec parent = N::FSpGetParent( genie );
	
	setenv( "SHELL",    N::FSpGetPOSIXPathname( genie             ).c_str(), 0 );
	setenv( "SHELLDIR", N::FSpGetPOSIXPathname( parent & ""       ).c_str(), 0 );
	setenv( "WISHES",   N::FSpGetPOSIXPathname( parent & "Wishes" ).c_str(), 0 );
	
	setenv( "PATH", getenv( "WISHES" ), 0 );
}

int O::Main( int argc, char const *const argv[] )
{
	setsid();  // New session
	setctty( STDIN_FILENO );  // Reattach to terminal
	
	SetVariables();
	
	char* exec_argv[] = { "-sh", NULL };
	
	int result = execv( "/bin/sh", exec_argv );
	
	if ( result == -1 )
	{
		_exit( result );
	}
	
	return result;
}

