/*
	nohup.cc
	--------
*/

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h>

// Extended API Set, Part 2
#include "extended-api-set/part-2.h"

// more-posix
#include "more/perror.hh"


#pragma exceptions off


#define NOHUP_OUT  "nohup.out"

#define STRLEN( s )  (sizeof "" s - 1)
#define STR_LEN( s ) "" s, STRLEN( s )

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( msg "\n" ) )

#define LENGTH( array )  (sizeof array / sizeof array[0])


int main( int argc, char *const argv[] )
{
	if ( argc < 2 )
	{
		WARN( "Usage: nohup command [ arg1 ... argn ]" );
		
		return 125;
	}
	
	struct sigaction action = {};
	
	action.sa_handler = SIG_IGN;
	
	sigaction( SIGHUP, &action, NULL );
	
	if ( isatty( STDOUT_FILENO ) )
	{
		const char* path = NULL;
		
		const int create_append = O_WRONLY | O_CREAT | O_APPEND;
		
		int output = open( NOHUP_OUT, create_append, 0600 );
		
		if ( output < 0 )
		{
			if ( const char* home = getenv( "HOME" ) )
			{
				path = home;
				
				int home_fd = open( home, O_RDONLY | O_DIRECTORY );
				
				if ( home_fd >= 0 )
				{
					output = openat( home_fd, NOHUP_OUT, create_append, 0600 );
					
					int saved_errno = errno;
					
					close( home_fd );
					
					errno = saved_errno;
				}
			}
			
			if ( output < 0 )
			{
				more::perror( "nohup: can't open a nohup.out file." );
				
				return 30;
			}
		}
		
		struct iovec output_message[] =
		{
			{ (char*) STR_LEN( "sending output to " ) },
			{ (char*) STR_LEN( ""                   ) },
			{ (char*) STR_LEN( ""                   ) },
			{ (char*) STR_LEN( NOHUP_OUT            ) },
			{ (char*) STR_LEN( "\n"                 ) }
		};
		
		if ( path )
		{
			output_message[1].iov_base = (char*) path;
			output_message[1].iov_len  = strlen( path );
			
			output_message[2].iov_base = (char*) "/";
			output_message[2].iov_len  = STRLEN( "/" );
		}
		
		writev( STDERR_FILENO, output_message, LENGTH( output_message ) );
		
		dup2( output, STDOUT_FILENO );
		
		close( output );
	}
	
	if ( isatty( STDERR_FILENO ) )
	{
		dup2( STDOUT_FILENO, STDERR_FILENO );
	}
	
	(void) execvp( argv[ 1 ], argv + 1 );
	
	bool noSuchFile = errno == ENOENT;
	
	more::perror( argv[0], argv[1] );
	
	return noSuchFile ? 127 : 126;
}
