/*
	fifo-wait.cc
	------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// more-posix
#include "more/perror.hh"


#define PROGRAM  "fifo-wait"

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
	
	int fd = open( path, O_WRONLY );
	
	if ( fd < 0 )
	{
		more::perror( PROGRAM, path );
		
		return 1;
	}
	
	close( fd );
	
	return 0;
}
