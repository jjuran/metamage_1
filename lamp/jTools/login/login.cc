/*	========
 *	login.cc
 *	========
 */

// Standard C
#include "stdlib.h"

// POSIX
#include "fcntl.h"
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
	
	setenv( "SHELL",    N::FSpGetPOSIXPathname( genie       ).c_str(), 0 );
	setenv( "SHELLDIR", N::FSpGetPOSIXPathname( parent & "" ).c_str(), 0 );
	
	const char* path =  "/usr/local/sbin"
	                   ":/usr/local/bin"
	                   ":/usr/sbin"
	                   ":/usr/bin"
	                   ":/sbin"
	                   ":/bin";
	
	setenv( "PATH", path, 0 );
}

static void DumpMOTD()
{
	int fd = open( "/etc/motd", O_RDONLY );
	
	if ( fd != -1 )
	{
		while ( true )
		{
			char data[ 4096 ];
			
			int bytes = read( fd, data, 4096 );
			
			if ( bytes <= 0 )
			{
				break;
			}
			
			bytes = write( STDOUT_FILENO, data, bytes );
		}
		
		(void)close( fd );
	}
}

int O::Main( int argc, char const *const argv[] )
{
	// Error output may be going to a log somewhere
	dup2( STDOUT_FILENO, STDERR_FILENO );
	
	setsid();  // New session
	setctty( STDIN_FILENO );  // Reattach to terminal
	
	SetVariables();
	
	DumpMOTD();
	
	char* exec_argv[] = { "-sh", NULL };
	
	int result = execv( "/bin/sh", exec_argv );
	
	if ( result == -1 )
	{
		_exit( result );
	}
	
	return result;
}

