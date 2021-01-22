/*
	hello-fifo.cc
	-------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// Standard C
#include <stdio.h>


#define HELLO  "Hello world\n"

int main()
{
	const char* path = "hello.fifo";
	
	int nok = mkfifo( path, 0666 );
	
	if ( nok < 0 )
	{
		perror( path );
		return 1;
	}
	
	int fd = -1;
	
	pid_t pid = fork();
	
	if ( pid < 0 )
	{
		perror( "fork" );
		return 1;
	}
	
	fd = open( path, pid ? O_RDONLY : O_WRONLY );
	
	if ( fd < 0 )
	{
		perror( path );
		return 1;
	}
	
	if ( pid )
	{
		char buffer[ 16 ];
		
		ssize_t n = read( fd, buffer, sizeof buffer );
		
		if ( n < 0 )
		{
			perror( "read" );
			return 1;
		}
		
		write( STDOUT_FILENO, buffer, n );
	}
	else
	{
		ssize_t n = write( fd, HELLO, sizeof HELLO - 1 );
		
		if ( n < 0 )
		{
			perror( "write" );
			return 1;
		}
		
		_exit( 0 );
	}
	
	unlink( path );
	
	return 0;
}
