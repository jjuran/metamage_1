/*
	interact-shim.cc
	----------------
*/

// POSIX
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

// Standard C
#include <signal.h>
#include <stdlib.h>

// more-posix
#include "more/perror.hh"


#define PROGRAM  "interact-shim"

#define USAGE "usage: GRAPHICS_UPDATE_SIGNAL_FIFO=<update-fifo> " PROGRAM "\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)


static const char* update_fifo = getenv( "GRAPHICS_UPDATE_SIGNAL_FIFO" );

static
void sigterm( int )
{
	/*
		We've been terminated.  Open the FIFO to unblock update_loop().
	*/
	
	open( update_fifo, O_RDONLY | O_NONBLOCK );
}

int main( int argc, char** argv )
{
	if ( update_fifo == NULL )
	{
		write( STDERR_FILENO, STR_LEN( USAGE ) );
		return 2;
	}
	
	struct stat st;
	int nok = stat( update_fifo, &st );
	
	if ( nok )
	{
		more::perror( PROGRAM, update_fifo );
		
		return 1;
	}
	
	if ( ! S_ISFIFO( st.st_mode ) )
	{
		more::perror( PROGRAM, update_fifo, "not a named pipe" );
		
		return 1;
	}
	
	signal( SIGTERM, &sigterm );
	
	/*
		Send a NUL byte.  This may be intercepted by exhibit to indicate
		readiness, or passed on to the raster author as a null SPIEL message.
	*/
	
	write( STDOUT_FILENO, "", 1 );
	
	int fd;
	
	while ( (fd = open( update_fifo, O_WRONLY )) >= 0 )
	{
		close( fd );
	}
	
	return 0;
}
