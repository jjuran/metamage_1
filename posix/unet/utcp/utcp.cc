/*
	utcp.cc
	-------
*/

#include "unet/pass_fd.hh"

// POSIX
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>

// Standard C
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// gear
#include "gear/parse_decimal.hh"

// poseven
#include "poseven/bundles/inet.hh"


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

static in_addr_t resolve_hostname( const char* hostname )
{
	hostent* hosts = gethostbyname( hostname );
	
	if ( !hosts  ||  h_errno )
	{
		fprintf( stderr, PROGRAM " hostname error (%d): %s\n", h_errno, hostname );
		
		exit( 1 );
	}
	
	in_addr addr = *(in_addr*) hosts->h_addr;
	
	return addr.s_addr;
}

static int connect( const char* host, int port )
{
	const p7::in_addr_t addr = p7::in_addr_t( resolve_hostname( host ) );
	
	return p7::connect( addr, p7::in_port_t( port ) ).release();
}

static int connect( const char* host, const char* port )
{
	return connect( host, gear::parse_unsigned_decimal( port ) );
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
	
	int remote_fd = connect( host, port );
	
	CHECK_N( unet::send_fd( socket_fd, remote_fd ) );
	CHECK_N( unet::send_fd( socket_fd, remote_fd ) );
	
	return 0;
}
