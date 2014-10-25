/*
	uexec.cc
	--------
*/

#include "unet/pass_fd.hh"

// POSIX
#include <unistd.h>
#include <sys/socket.h>

// Standard C
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define PROGRAM  "uexec"

#define USAGE  "usage: " PROGRAM " server-path [args...]\n"  \
               "       (stdout must be a local socket)\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)


#define CHECK_N( call )  check_n( call, #call )

static inline int check_n( int result, const char* code )
{
	if ( result < 0 )
	{
		fprintf( stderr, PROGRAM " error: %s: %s\n", code, strerror( errno ) );
		
		exit( 1 );
	}
	
	return result;
}

static int usage()
{
	write( STDERR_FILENO, STR_LEN( USAGE ) );
	
	return 2;
}

int main( int argc, char** argv )
{
	const int socket_fd = 1;  // STDOUT_FILENO
	
	if ( argc < 1 + 1 )
	{
		return usage();
	}
	
	int fds[ 2 ];
	
	CHECK_N( socketpair( PF_LOCAL, SOCK_STREAM, 0, fds ) );
	
	const int remote_fd = fds[ 0 ];
	
	CHECK_N( unet::send_fd( socket_fd, remote_fd ) );
	CHECK_N( unet::send_fd( socket_fd, remote_fd ) );
	
	// Let the parent know we're going to stay running
	
	char c = '\0';
	
	CHECK_N( write( socket_fd, &c, sizeof c ) );
	
	CHECK_N( dup2( fds[ 1 ], 0 ) );
	CHECK_N( dup2( fds[ 1 ], 1 ) );
	
	CHECK_N( close( fds[ 0 ] ) );
	CHECK_N( close( fds[ 1 ] ) );
	
	char** args = argv + 1;
	
	(void) execvp( args[ 0 ], args );
	
	const int exit_code = errno == ENOENT ? 127 : 126;
	
	return exit_code;
}
