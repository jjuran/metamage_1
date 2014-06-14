/*
	fifo-signal.cc
	--------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// more-posix
#include "more/perror.hh"


#define PROGRAM  "fifo-signal"

#define USAGE  "usage: " PROGRAM " path\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)


static int usage()
{
	write( STDERR_FILENO, STR_LEN( USAGE ) );
	
	return 2;
}

int main( int argc, char** argv )
{
	if ( argc != 1 + 1 )
	{
		return usage();
	}
	
	const char* path = argv[ 1 ];
	
	int fd = open( path, O_RDONLY | O_NONBLOCK );
	
	if ( fd < 0 )
	{
		more::perror( PROGRAM, path );
		
		return 1;
	}
	
	char buffer[ 1 ];
	
	ssize_t n_read = read( fd, buffer, sizeof buffer );
	
	if ( n_read < 0 )
	{
		if ( errno != EAGAIN )
		{
			more::perror( PROGRAM, "read" );
			
			return 1;
		}
		
		write( STDOUT_FILENO, STR_LEN( "waiters unblocked\n" ) );
	}
	
	close( fd );
	
	return 0;
}
