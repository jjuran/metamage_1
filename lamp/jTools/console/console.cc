/*	==========
 *	console.cc
 *	==========
 */

// Standard C/C++
#include <cstring>

// POSIX
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/ttycom.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vfork.h>

// Lamp
#include "lamp/winio.h"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


#define STR_LEN( str )  "" str, (sizeof str - 1)


namespace O = Orion;


static int exit_from_wait( int stat )
{
	int result = WIFEXITED( stat )   ? WEXITSTATUS( stat )
	           : WIFSIGNALED( stat ) ? WTERMSIG( stat ) + 128
	           :                       -1;
	
	return result;
}

int O::Main( int argc, char const *const argv[] )
{
	bool should_wait = false;
	
	const char* title = NULL;
	
	const char* device = "/dev/new/console";
	
	O::BindOption( "-d", device      );
	O::BindOption( "-t", title       );
	O::BindOption( "-w", should_wait );
	
	O::AliasOption( "-d", "--dev"   );
	O::AliasOption( "-t", "--title" );
	O::AliasOption( "-w", "--wait"  );
	
	O::GetOptions( argc, argv );
	
	char const *const *freeArgs = O::FreeArguments();
	
	if ( *freeArgs == NULL )
	{
		(void) write( STDERR_FILENO, STR_LEN( "Usage: console command [ arg1 ... argn ]\n" ) );
		
		return 1;
	}
	
	int forked = vfork();
	
	if ( forked == 0 )
	{
		// New child, so we're not a process group leader
		
		setsid();
		
		int console = open( device, O_RDWR, 0 );
		
		int io = ioctl( console, TIOCSCTTY, NULL );
		
		if ( title != NULL )
		{
			io = ioctl( console, WIOCSTITLE, (int*) title );
		}
		
		dup2( console, 0 );
		dup2( console, 1 );
		dup2( console, 2 );
		
		close( console );
		
		(void) execvp( freeArgs[ 0 ], &freeArgs[ 0 ] );
		
		_exit( 127 );
	}
	
	if ( should_wait )
	{
		int stat = -1;
		
		int waited = waitpid( forked, &stat, 0 );
		
		if ( waited == -1 )
		{
			std::perror( "console: waitpid" );
			
			return 127;
		}
		
		return exit_from_wait( stat );
	}
	
	return 0;
}

