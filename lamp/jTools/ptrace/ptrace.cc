/*	=========
 *	ptrace.cc
 *	=========
 */

// Standard C++
#include <string>

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

// POSIX
#include <sys/ptrace.h>
#include <unistd.h>


#define STR_LEN( str )  "" str, (sizeof str - 1)


#pragma export on

int main( int argc, char const *const argv[] )
{
	if ( argc < 2 )
	{
		(void) write( STDERR_FILENO, STR_LEN( "Usage: ptrace command [ arg1 ... argn ]\n" ) );
		
		return 1;
	}
	
	int traced = ptrace( PTRACE_TRACEME, 0, NULL, 0 );
	
	(void) execvp( argv[ 1 ], argv + 1 );
	
	bool noSuchFile = errno == ENOENT;
	
	std::fprintf( stderr, "%s: %s: %s\n", argv[0], argv[1], std::strerror( errno ) );
	
	return noSuchFile ? 127 : 126;
}

#pragma export reset

