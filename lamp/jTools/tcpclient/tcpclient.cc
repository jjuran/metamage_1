/*
	tcpclient.cc
	------------
*/

// POSIX
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

// Standard C
#include <errno.h>

// more-posix
#include "more/perror.hh"


#pragma exceptions off


#define PROGRAM  "tcpclient"

#define USAGE  "usage: " PROGRAM " <host> <port> <program argv>\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)


class addrinfo_cleanup
{
	private:
		addrinfo* its_ai;
		
		// non-copyable
		addrinfo_cleanup           ( const addrinfo_cleanup& );
		addrinfo_cleanup& operator=( const addrinfo_cleanup& );
	
	public:
		addrinfo_cleanup( addrinfo* ai ) : its_ai( ai )
		{
		}
		
		~addrinfo_cleanup()
		{
			::freeaddrinfo( its_ai );
		}
};

static inline
int gai_error( const char* node, const char* service, int err )
{
	const char* error = gai_strerror( err );
	
	more::perror( PROGRAM, node, error );
	
	return -1;
}

static
int tcp_connect( const char* node, const char* service )
{
	addrinfo hints = {};
	
	hints.ai_socktype = SOCK_STREAM;
	
	addrinfo* ai;
	
	int err = getaddrinfo( node, service, &hints, &ai );
	
	if ( err )
	{
		return gai_error( node, service, err );
	}
	
	addrinfo_cleanup cleanup( ai );
	
	int fd = socket( ai->ai_family, SOCK_STREAM, 0 );
	
	if ( fd < 0 )
	{
		more::perror( PROGRAM, "socket" );
		
		return fd;
	}
	
	int nok = connect( fd, ai->ai_addr, ai->ai_addrlen );
	
	if ( nok < 0 )
	{
		more::perror( PROGRAM, node );
		
		close( fd );
		
		return -1;
	}
	
	return fd;
}

int main( int argc, char** argv )
{
	if ( argc < 4 )
	{
		write( STDERR_FILENO, STR_LEN( USAGE ) );
		
		return 1;
	}
	
	const char* hostname = argv[ 1 ];
	const char* port_str = argv[ 2 ];
	
	int fd = tcp_connect( hostname, port_str );
	
	if ( fd < 0 )
	{
		return 1;  // errors already reported
	}
	
	const int tcp_in  = 6;
	const int tcp_out = 7;
	
	if ( dup2( fd, tcp_in ) < 0  ||  dup2( fd, tcp_out ) < 0 )
	{
		more::perror( PROGRAM, "dup2" );
		
		return 1;
	}
	
	char** program_argv = argv + 3;
	
	execvp( program_argv[ 0 ], program_argv );
	
	return errno == ENOENT ? 127 : 126;
}
