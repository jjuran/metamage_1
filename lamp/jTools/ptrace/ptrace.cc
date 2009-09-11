/*	=========
 *	ptrace.cc
 *	=========
 */

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

// POSIX
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

// Iota
#include "iota/strings.hh"

// more-posix
#include "more/perror.hh"


#pragma exceptions off


int main( int argc, char *const argv[] )
{
	if ( argc < 2 )
	{
		(void) write( STDERR_FILENO, STR_LEN( "Usage: ptrace command [ arg1 ... argn ]\n" ) );
		
		return 1;
	}
	
	pid_t pid = vfork();
	
	if ( pid == -1 )
	{
		more::perror( "vfork" );
		
		return 127;
	}
	
	if ( pid == 0 )
	{
		int traced = ptrace( PTRACE_TRACEME, 0, NULL, 0 );
		
		(void) execvp( argv[ 1 ], argv + 1 );
		
		_exit( 127 );
	}
	
	int stat = -1;
	
	while ( true )
	{
		int waited = waitpid( pid, &stat, 0 );
		
		if ( waited < 0 )
		{
			more::perror( "ptrace: waitpid" );
			
			return EXIT_FAILURE;
		}
		
		if ( WIFSTOPPED( stat ) )
		{
			write( STDOUT_FILENO, STR_LEN( "ptrace: target stopped (hit return to continue)\n" ) );
			
			char c;
			
			ssize_t bytes = read( STDIN_FILENO, &c, sizeof c );
			
			ptrace( PTRACE_CONT, pid, NULL, 0 );
		}
		else
		{
			break;
		}
	}
	
	return 0;
}

