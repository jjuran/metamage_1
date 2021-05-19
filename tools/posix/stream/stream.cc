/*
	stream.cc
	---------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <string.h>

// more-posix
#include "more/perror.hh"

// posix-utils
#include "posix/open_or_connect.hh"


#define PROGRAM  "stream"

#define USAGE "Usage: stream file-path\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)


static
int usage()
{
	write( STDERR_FILENO, STR_LEN( USAGE ) );
	return 1;
}

static inline
void report_error( const char* path )
{
	more::perror( PROGRAM, path );
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
		report_error( path );
		return 1;
	}
	
	if ( fd != stdio )
	{
		close( fd );
	}
	
	const char* cat = "/bin/cat";
	
	execl( cat, "cat", NULL );
	
	report_error( cat );
	return 1;
}
