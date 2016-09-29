/*
	stream.cc
	---------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <stdio.h>

// posix-utils
#include "posix/open_or_connect.hh"


#define USAGE "Usage: stream file-path\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)


static
int usage()
{
	write( STDERR_FILENO, STR_LEN( USAGE ) );
	return 1;
}

int main( int argc, char** argv )
{
	if ( argc != 2 )
	{
		return usage();
	}
	
	const char* path = argv[ 1 ];
	
	int fd = posix::open_or_connect( path, O_RDONLY, 0 );
	
	if ( fd < 0  ||  dup2( fd, STDIN_FILENO ) < 0 )
	{
		perror( path );
		return 1;
	}
	
	if ( fd != STDIN_FILENO )
	{
		close( fd );
	}
	
	const char* cat = "/bin/cat";
	
	execl( cat, "cat", NULL );
	
	perror( cat );
	return 1;
}
