/*
	uunix.cc
	--------
*/

#include "unet/pass_fd.hh"

// POSIX
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

// Standard C
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define PROGRAM  "uunix"

#define USAGE  "usage: " PROGRAM " path\n"  \
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

static int connect( const char* path )
{
	int fd = CHECK_N( socket( PF_LOCAL, SOCK_STREAM, 0 ) );
	
	sockaddr_un un = { 0 };
	
	un.sun_family = AF_LOCAL;
	
	const size_t length = strlen( path );
	
	if ( length >= sizeof un.sun_path )
	{
		errno = ENAMETOOLONG;
		
		check_n( -1, path );
	}
	
	memcpy( un.sun_path, path, length + 1 );
	
	CHECK_N( connect( fd, (const sockaddr*) &un, sizeof un ) );
	
	return fd;
}

static int usage()
{
	write( STDERR_FILENO, STR_LEN( USAGE ) );
	
	return 2;
}

int main( int argc, char** argv )
{
	const int socket_fd = 1;  // STDOUT_FILENO
	
	if ( argc != 1 + 1 )
	{
		return usage();
	}
	
	const char* path = argv[ 1 ];
	
	int remote_fd = connect( path );
	
	CHECK_N( unet::send_fd( socket_fd, remote_fd ) );
	CHECK_N( unet::send_fd( socket_fd, remote_fd ) );
	
	return 0;
}
