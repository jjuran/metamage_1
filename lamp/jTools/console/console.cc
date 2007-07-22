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

// Lamp
#include "lamp/winio.h"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


#define STR_LEN( str )  "" str, (sizeof str - 1)


namespace O = Orion;


int O::Main( int argc, char const *const argv[] )
{
	bool detach = false;
	
	const char* title = NULL;
	
	O::BindOption( "-d", detach );
	O::BindOption( "-t", title  );
	
	O::AliasOption( "-d", "--detach" );
	O::AliasOption( "-t", "--title"  );
	
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
		
		int console = open( "/dev/new/console", O_RDWR, 0 );
		
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
	
	if ( !detach )
	{
		int stat = -1;
		
		int waited = waitpid( forked, &stat, 0 );
	}
	
	return 0;
}

