/*
	unixclient.cc
	-------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// more-posix
#include "more/perror.hh"

// posix-utils
#include "posix/open_or_connect.hh"


#define PROGRAM  "unixclient"

#define USAGE "Usage: unixclient file-path program [args...]\n"

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
	if ( argc < 3 )
	{
		return usage();
	}
	
	const char* socket_path = argv[ 1 ];
	
	char** program_argv = argv + 2;
	
	int fd = posix::open_or_connect( socket_path, O_RDWR, 0 );
	
	if ( fd < 0  ||  dup2( fd, 6 ) < 0  ||  dup2( fd, 7 ) < 0 )
	{
		report_error( socket_path );
		return 1;
	}
	
	if ( fd != 6  &&  fd != 7 )
	{
		close( fd );
	}
	
	execvp( program_argv[ 0 ], program_argv );
	
	report_error( program_argv[ 0 ] );
	return 1;
}
