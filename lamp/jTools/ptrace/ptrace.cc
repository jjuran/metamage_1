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
#include <sys/wait.h>
#include <unistd.h>
#include <vfork.h>


#define STR_LEN( str )  "" str, (sizeof str - 1)


#pragma export on

int main( int argc, char const *const argv[] )
{
	if ( argc < 2 )
	{
		(void) write( STDERR_FILENO, STR_LEN( "Usage: ptrace command [ arg1 ... argn ]\n" ) );
		
		return 1;
	}
	
	pid_t pid = vfork();
	
	if ( pid == -1 )
	{
		std::perror( "vfork" );
		
		return 127;
	}
	
	if ( pid == 0 )
	{
		int traced = ptrace( PTRACE_TRACEME, 0, NULL, 0 );
		
		(void) execvp( argv[ 1 ], argv + 1 );
		
		_exit( 127 );
	}
	
	int stat = -1;
	
	int waited = waitpid( pid, &stat, 0 );
	
	return 0;
}

#pragma export reset

