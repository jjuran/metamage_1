/*
	tcpclient.cc
	------------
*/

// POSIX
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

// Standard C
#include <errno.h>

// more-posix
#include "more/perror.hh"

// command
#include "command/get_option.hh"


#pragma exceptions off


#define PROGRAM  "tcpclient"

#define USAGE  "usage: " PROGRAM " [-Dd] <host> <port> <program argv>\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)


using namespace command::constants;


enum
{
	Option_no_delay = 'D',
	Option_delay    = 'd',
};

static command::option options[] =
{
	{ "delay",    Option_delay    },
	{ "no-delay", Option_no_delay },
	
	{ NULL }
};

static int delay = true;

static
char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_delay:
				delay = true;
				break;
						
			case Option_no_delay:
				delay = false;
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

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
	
	if ( ! delay )
	{
		int value = 1;
		
		nok = setsockopt( fd, IPPROTO_TCP, TCP_NODELAY, &value, sizeof value );
		
		if ( nok < 0 )
		{
			more::perror( PROGRAM, "TCP_NODELAY" );
			
			// not a critical error; continue
		}
	}
	
	return fd;
}

int main( int argc, char** argv )
{
	char *const *args = get_options( argv );
	
	int argn = argc - (args - argv);
	
	if ( argn < 3 )
	{
		write( STDERR_FILENO, STR_LEN( USAGE ) );
		
		return 1;
	}
	
	const char* hostname = args[ 0 ];
	const char* port_str = args[ 1 ];
	
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
	
	char** program_argv = (char**) args + 2;
	
	execvp( program_argv[ 0 ], program_argv );
	
	return errno == ENOENT ? 127 : 126;
}
