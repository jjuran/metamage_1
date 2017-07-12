/*
	utcp.cc
	-------
*/

#include "unet/pass_fd.hh"

// POSIX
#include <netdb.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// poseven
#include "poseven/bundles/inet.hh"
#include "poseven/types/gai_error.hh"


#define PROGRAM  "utcp"

#define USAGE  "usage: " PROGRAM " host port\n"  \
               "       " PROGRAM " host:port\n"  \
               "       (stdout must be a local socket)\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)


namespace p7 = poseven;


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
	
	if ( argc < 1 + 1  ||  argc > 2 + 1 )
	{
		return usage();
	}
	
	const char* host = argv[ 1 ];
	const char* port = argv[ 2 ];
	
	if ( port == NULL )
	{
		if ( char* colon = strchr( argv[ 1 ], ':' ) )
		{
			*colon = '\0';
			
			port = colon + 1;
		}
		else
		{
			return usage();
		}
	}
	
	int remote_fd;
	
	try
	{
		remote_fd = p7::connect( host, port ).release();
	}
	catch ( const p7::gai_error& err )
	{
		fprintf( stderr, "utcp: %s:%s: %s\n", host, port, gai_strerror( err ) );
		
		return 1;
	}
	catch ( const p7::errno_t& err )
	{
		fprintf( stderr, "utcp: %s:%s: %s\n", host, port, strerror( err ) );
		
		return 1;
	}
	
	CHECK_N( unet::send_fd( socket_fd, remote_fd ) );
	CHECK_N( unet::send_fd( socket_fd, remote_fd ) );
	
	return 0;
}
