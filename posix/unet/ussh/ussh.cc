/*
	ussh.cc
	-------
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


#define PROGRAM  "ussh"

#define USAGE  "usage: " PROGRAM " -- [user@]hostname command [args...]\n"  \
               "       (stdout must be a local socket)\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)


#ifdef __RELIX__
#define FORK() ::vfork()
#else
#define FORK() ::fork()
#endif


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

static int connect( const char** argv )
{
	int fds[ 2 ];
	
	CHECK_N( socketpair( PF_LOCAL, SOCK_STREAM, 0, fds ) );
	
	pid_t child = CHECK_N( FORK() );
	
	if ( child == 0 )
	{
		CHECK_N( close( fds[ 0 ] ) );
		
		CHECK_N( dup2( fds[ 1 ], 0 ) );
		CHECK_N( dup2( fds[ 1 ], 1 ) );
		
		argv[ 0 ] = "ssh";
		argv[ 1 ] = "-T";
		
		(void) execvp( argv[ 0 ], (char**) argv );
		
		const int exit_code = errno == ENOENT ? 127 : 126;
		
		_exit( exit_code );
	}
	
	CHECK_N( close( fds[ 1 ] ) );
	
	return fds[ 0 ];
}

static int usage()
{
	write( STDERR_FILENO, STR_LEN( USAGE ) );
	
	return 2;
}

int main( int argc, char** argv )
{
	const int socket_fd = 1;  // STDOUT_FILENO
	
	if ( argc < 3 + 1 )
	{
		return usage();
	}
	
	if ( strcmp( argv[ 1 ], "--" ) != 0 )
	{
		return usage();
	}
	
	int remote_fd = connect( (const char**) argv );
	
	CHECK_N( unet::send_fd( socket_fd, remote_fd ) );
	CHECK_N( unet::send_fd( socket_fd, remote_fd ) );
	
	return 0;
}
