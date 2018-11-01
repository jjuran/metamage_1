/*
	writer.cc
	---------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>

// more-posix
#include "more/perror.hh"

// pass_fd
#include "unet/pass_fd.hh"


#define PROGRAM  "writer"

#define USAGE "Usage: " PROGRAM " path\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)


static
int usage()
{
	write( STDERR_FILENO, STR_LEN( USAGE ) );
	return 2;
}

static inline
void report_error( const char* path, int err )
{
	more::perror( PROGRAM, path, err );
}

int main( int argc, char** argv )
{
	if ( argc != 2 )
	{
		return usage();
	}
	
	const char* path = argv[ 1 ];
	
	int writing_fd = open( path, O_WRONLY );
	
	if ( writing_fd < 0 )
	{
		report_error( path, errno );
		return 1;
	}
	
	int nok = unet::send_fd( STDOUT_FILENO, writing_fd );
	
	if ( nok )
	{
		report_error( "send_fd", errno );
		return 1;
	}
	
	return 0;
}
