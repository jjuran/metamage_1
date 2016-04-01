/*
	listen.cc
	---------
*/

// POSIX
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// gear
#include "gear/parse_decimal.hh"


#define PROGRAM  "listen"

#define USAGE "Usage: " PROGRAM " address command\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)


static int usage()
{
	write( STDERR_FILENO, STR_LEN( USAGE ) );
	return 1;
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

static int listen_inet( in_addr_t addr, in_port_t port )
{
	int s = socket( PF_INET, SOCK_STREAM, 0 );
	
	if ( s < 0 )
	{
		return s;
	}
	
	int on = 1;
	setsockopt( s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on );
	
	struct sockaddr_in in = { 0 };
	
	in.sin_family      = AF_INET;
	in.sin_port        = port;
	in.sin_addr.s_addr = addr;
	
	int nok = bind( s, (const sockaddr*) &in, sizeof in );
	
	if ( nok )
	{
		close( s );
		return nok;
	}
	
	nok = listen( s, 5 );
	
	if ( nok )
	{
		close( s );
		return nok;
	}
	
	return s;
}

static int listen_unix( const char* path )
{
	const size_t path_len = strlen( path );
	
	if ( path_len >= sizeof sockaddr_un().sun_path )
	{
		errno = ENAMETOOLONG;
		return -1;
	}
	
	int s = socket( PF_UNIX, SOCK_STREAM, 0 );
	
	if ( s < 0 )
	{
		return s;
	}
	
	/*
		We don't fully error-check the lstat() or unlink() calls below.
		The goal is to delete any existing socket file.  If a non-socket
		exists, or if something fails (like path resolution), the bind()
		call will catch it.
	*/
	
	struct stat st;
	int nok = lstat( path, &st );
	int ok = ! nok;
	
	if ( ok  &&  S_ISSOCK( st.st_mode ) )
	{
		unlink( path );
	}
	
	sockaddr_un un = { 0 };
	
	un.sun_family = AF_UNIX;
	
	memcpy( un.sun_path, path, path_len + 1 );
	
	nok = bind( s, (const sockaddr*) &un, sizeof un );
	
	if ( nok )
	{
		close( s );
		return nok;
	}
	
	nok = listen( s, 5 );
	
	if ( nok )
	{
		close( s );
		return nok;
	}
	
	return s;
}

static void spawn( int client_fd, char** argv )
{
	pid_t pid = vfork();
	
	if ( pid == 0 )
	{
		if ( dup2( client_fd, STDIN_FILENO  ) < 0 )  goto fail;
		if ( dup2( client_fd, STDOUT_FILENO ) < 0 )  goto fail;
		
		close( client_fd );
		
		execv( argv[ 0 ], argv );
		
	fail:
		
		int saved_errno = errno;
		
		perror( argv[ 0 ] );
		
		_exit( saved_errno == ENOENT ? 127 : 126 );
	}
}

static void event_loop( int listener_fd, char** argv )
{
	while ( true )
	{
		sockaddr_storage addr;
		socklen_t len = sizeof addr;
		
		int client_fd = accept( listener_fd, (sockaddr*) &addr, &len );
		
		if ( client_fd < 0 )
		{
			return;
		}
		
		spawn( client_fd, argv );
		
		close( client_fd );
	}
}

static char* find_first_of_two( char* p, char one, char two )
{
	while ( *p != one  &&  *p != two )
	{
		++p;
	}
	
	return p;
}

int main( int argc, char** argv )
{
	char** args = argv + 1;
	
	int argn = argc - 1;
	
	if ( argn < 2 )
	{
		return usage();
	}
	
	char* addr = args[ 0 ];
	
	char* colon = NULL;
	
	const char* port_arg = NULL;
	
	if ( addr[ 0 ] == ':' )
	{
		port_arg = addr + 1;
		
		addr = (char*) "0.0.0.0";
	}
	else
	{
		char* it = find_first_of_two( addr, ':', '/' );
		
		if ( *it == ':' )
		{
			colon = it;
			*it++ = '\0';
			port_arg = it;
		}
	}
	
	write( STDOUT_FILENO, STR_LEN( "Daemon starting up..." ) );
	
	signal( SIGCHLD, SIG_IGN );
	
	int listener_fd;
	
	if ( port_arg != NULL )
	{
		in_addr_t host = resolve_hostname( addr );
		in_port_t port = htons( gear::parse_unsigned_decimal( port_arg ) );
		
		listener_fd = listen_inet( host, port );
	}
	else
	{
		listener_fd = listen_unix( addr );
	}
	
	if ( listener_fd < 0 )
	{
		write( STDOUT_FILENO, STR_LEN( " FAILED.\n" ) );
		
		if ( colon != NULL )
		{
			*colon = ':';
		}
		else
		{
			write( STDERR_FILENO, STR_LEN( "0.0.0.0" ) );
		}
		
		perror( args[ 0 ] );
		return 1;
	}
	
	write( STDOUT_FILENO, STR_LEN( " done.\n" ) );
	
	event_loop( listener_fd, args + 1 );
	
	close( listener_fd );
	
	return 0;
}
