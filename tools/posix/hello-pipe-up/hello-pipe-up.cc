/*
	hello-pipe-up.cc
	----------------
*/

// POSIX
#include <unistd.h>
#include <sys/wait.h>

// Standard C
#include <stdio.h>


#define HELLO  "Hello world\n"

int main()
{
	int pipe_fds[ 2 ];
	
	int nok = pipe( pipe_fds );
	
	if ( nok < 0 )
	{
		perror( "pipe" );
		return 1;
	}
	
	pid_t pid = fork();
	
	if ( pid < 0 )
	{
		perror( "fork" );
		return 1;
	}
	
	if ( pid )
	{
		char buffer[ 16 ];
		
		ssize_t n = read( pipe_fds[ 0 ], buffer, sizeof buffer );
		
		if ( n < 0 )
		{
			perror( "read" );
			return 1;
		}
		
		write( STDOUT_FILENO, buffer, n );
	}
	else
	{
		ssize_t n = write( pipe_fds[ 1 ], HELLO, sizeof HELLO - 1 );
		
		if ( n < 0 )
		{
			perror( "write" );
			return 1;
		}
		
		_exit( 0 );
	}
	
	int status;
	wait( &status );
	
	return 0;
}
