/*
	fork-abort.c
	------------
*/

// POSIX
#include <unistd.h>
#include <sys/wait.h>

// Standard C
#include <stdlib.h>


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define ERROR( msg )  write( STDERR_FILENO, STR_LEN( msg "\n" ) )
#define PRINT( msg )  write( STDOUT_FILENO, STR_LEN( msg "\n" ) )


int main( int argc, char** argv )
{
	pid_t pid = fork();
	
	if ( pid < 0 )
	{
		ERROR( "fork() failed" );
		return 1;
	}
	
	if ( pid == 0 )
	{
		PRINT( "child" );
	}
	else
	{
		int status;
		
		PRINT( "parent" );
		
		wait( &status );
	}
	
	abort();
	
	return 0;
}
