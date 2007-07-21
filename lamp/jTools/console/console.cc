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


#pragma exceptions off


#define STR_LEN( str )  "" str, (sizeof str - 1)

#pragma export on

int main( int argc, char const *const argv[] )
{
	if ( argc < 2 )
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
		
		dup2( console, 0 );
		dup2( console, 1 );
		dup2( console, 2 );
		
		close( console );
		
		(void) execvp( argv[ 1 ], argv + 1 );
		
		_exit( 127 );
	}
	
	int stat = -1;
	
	int waited = waitpid( forked, &stat, 0 );
	
	return 0;
}

#pragma export reset

