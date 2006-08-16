/*	========
 *	login.cc
 *	========
 */

// Standard C
#include "stdlib.h"

// POSIX
#include "fcntl.h"
#include "sys/ioctl.h"
#include "unistd.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Orion
#include "Orion/Main.hh"


namespace N = Nitrogen;
namespace O = Orion;


static void SetVariables()
{
	#if TARGET_CPU_68K
	#define ENV_HOSTTYPE "m68k"
	#elif TARGET_CPU_PPC
	#define ENV_HOSTTYPE "powerpc"
	#endif
	
	#if TARGET_RT_MAC_CFM
	#define ENV_MAC_RUNTIME "cfm"
	#elif TARGET_RT_MAC_MACHO
	#define ENV_MAC_RUNTIME "mach-o"
	#else
	#define ENV_MAC_RUNTIME "rsrc"
	#endif
	
	#if TARGET_API_MAC_CARBON
	#define ENV_MAC_BACKEND "carbon"
	#else
	#define ENV_MAC_BACKEND "classic"
	#endif
	
	setenv( "HOSTTYPE", ENV_HOSTTYPE               , 0 );
	setenv( "OSTYPE",                        "lamp", 0 );
	setenv( "MACHTYPE", ENV_HOSTTYPE "-jtools-lamp", 0 );
	
	setenv( "MAC_RUNTIME", ENV_MAC_RUNTIME, 0 );
	setenv( "MAC_BACKEND", ENV_MAC_BACKEND, 0 );
	
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
	ioctl( STDIN_FILENO, TIOCSCTTY, NULL );  // Reattach to terminal
	
	SetVariables();
	
	DumpMOTD();
	
	char* exec_argv[] = { "-sh", NULL };
	
	int result = execv( "/bin/sh", exec_argv );
	
	_exit( result );
	
	return 0;  // Not reached
}

