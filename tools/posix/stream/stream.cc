/*
	stream.cc
	---------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <stdio.h>
#include <string.h>

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
	bool into;
	
	if ( argc < 2  ||  argc != 2 + (into = strcmp( argv[ 1 ], "--into" ) == 0) )
	{
		return usage();
	}
	
	const char* path = argv[ 1 + into ];
	
	const int flags = O_RDONLY + (O_WRONLY - O_RDONLY) * into;
	const int stdio = STDIN_FILENO + into;
	
	int fd = posix::open_or_connect( path, flags, 0 );
	
	if ( fd < 0  ||  dup2( fd, stdio ) < 0 )
	{
		perror( path );
		return 1;
	}
	
	if ( fd != stdio )
	{
		close( fd );
	}
	
	const char* cat = "/bin/cat";
	
	execl( cat, "cat", NULL );
	
	perror( cat );
	return 1;
}
