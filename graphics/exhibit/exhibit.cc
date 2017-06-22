/*
	exhibit.cc
	----------
*/

// POSIX
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// more-posix
#include "more/perror.hh"

// command
#include "command/get_option.hh"

// gear
#include "gear/parse_decimal.hh"

// poseven
#include "poseven/bundles/inet.hh"


#define VIEWER         "vraster"
#define INTERACT       "interact"
#define TELECAST_SEND  "telecast-send"

#define PROGRAM  "exhibit"

#define USAGE  "usage: " PROGRAM " --raster <raster-path> <raster-author-program>\n" \
"       where raster-path is a raster file\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define WARN( msg )  write( STDERR_FILENO, STR_LEN( PROGRAM ": " msg "\n" ) )


namespace p7 = poseven;


enum
{
	Opt_title   = 't',
	Opt_magnify = 'x',
	
	Opt_last_byte = 255,
	
	Opt_raster,
};

static command::option options[] =
{
	{ "raster",  Opt_raster,  command::Param_required },
	{ "title",   Opt_title,   command::Param_required },
	{ "magnify", Opt_magnify, command::Param_required },
	{ NULL }
};


static const char* raster_path;
static const char* title = "";

static const char* magnifier = "1";


static pid_t raster_feed_pid   = 0;
static pid_t telecast_send_pid = 0;
static pid_t viewer_pid        = 0;  // either interact or vraster
static pid_t author_pid        = 0;

static int feed_fd   = -1;
static int server_fd = -1;
static int telecast_feed_fd = -1;
static int interact_feed_fd = -1;

static sig_atomic_t child_terminated = 0;


static
void sigchld( int )
{
	++child_terminated;
}

static inline
void report_error( const char* path, int err )
{
	more::perror( PROGRAM, path, err );
}

static
char* const* get_options( char** argv )
{
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		using command::global_result;
		
		switch ( opt )
		{
			case Opt_raster:
				raster_path = global_result.param;
				break;
			
			case Opt_title:
				title = global_result.param;
				break;
			
			case Opt_magnify:
				magnifier = global_result.param;
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

static
void init_raster( const char* raster_path )
{
	pid_t pid = fork();
	
	if ( pid < 0 )
	{
		report_error( "fork", errno );
		exit( 1 );
	}
	
	if ( pid == 0 )
	{
		const char* argv[] = { "raster", "init", raster_path, NULL };
		
		execvp( *argv, (char**) argv );
		
		const int saved_errno = errno;
		
		report_error( *argv, saved_errno );
		
		_exit( saved_errno == ENOENT ? 127 : 126 );
	}
	
	int wait_status;
	pid_t child_pid = wait( &wait_status );
	
	if ( child_pid < 0 )
	{
		report_error( "wait", errno );
		exit( 1 );
	}
	
	if ( WIFSIGNALED( wait_status ) )
	{
		WARN( "raster init: fatal signal" );
		exit( 1 );
	}
	
	if ( ! WIFEXITED( wait_status ) )
	{
		WARN( "raster init: non-signal, non-exit termination, and I'm confused" );
		exit( 1 );
	}
	
	if ( const int exit_status = WEXITSTATUS( wait_status ) )
	{
		WARN( "raster init failed" );
		exit( 1 );
	}
}

static
p7::in_addr_t resolve_hostname( const char* hostname )
{
	hostent* hosts = gethostbyname( hostname );
	
	if ( hosts == NULL  ||  h_errno != 0 )
	{
		fprintf( stderr, PROGRAM ": %s: hostname lookup error\n", hostname );
		exit( 1 );
	}
	
	in_addr addr = *(in_addr*) hosts->h_addr;
	
	return p7::in_addr_t( addr.s_addr );
}

static
int connect_to_server( char* addr )
{
	unsigned port = 0;
	
	if ( char* colon = strchr( addr, ':' ) )
	{
		*colon++ = '\0';
		
		port = gear::parse_unsigned_decimal( colon );
		
		if ( port > 0xFFFF )
		{
			fprintf( stderr, PROGRAM ": Port number %u is too large!\n", port );
			exit( 2 );
		}
	}
	
	try
	{
		return p7::connect( resolve_hostname( addr ), p7::in_port_t( port ) ).release();
	}
	catch ( const p7::errno_t& err )
	{
		report_error( addr, err );
		exit( 1 );
	}
	
	return 0;  // not reached
}

static
void launch_raster_feed( const char* raster_path )
{
	int fds[ 2 ];
	
	int nok = pipe( fds );
	
	if ( nok )
	{
		report_error( "pipe", errno );
		exit( 1 );
	}
	
	raster_feed_pid = fork();
	
	if ( raster_feed_pid < 0 )
	{
		report_error( "fork", errno );
		exit( 1 );
	}
	
	if ( raster_feed_pid == 0 )
	{
		dup2( fds[ 1 ], STDOUT_FILENO );
		
		close( fds[ 0 ] );
		close( fds[ 1 ] );
		
		const char* argv[] = { "raster", "feed", raster_path, NULL };
		
		execvp( *argv, (char**) argv );
		
		const int saved_errno = errno;
		
		report_error( *argv, saved_errno );
		
		_exit( saved_errno == ENOENT ? 127 : 126 );
	}
	
	close( fds[ 1 ] );
	
	feed_fd = fds[ 0 ];
}

static
void launch_telecast_send( int server_fd, const char* raster_path )
{
	int fds[ 2 ];
	
	int nok = pipe( fds );
	
	if ( nok )
	{
		report_error( "pipe", errno );
		exit( 1 );
	}
	
	telecast_send_pid = fork();
	
	if ( telecast_send_pid < 0 )
	{
		report_error( "fork", errno );
		exit( 1 );
	}
	
	if ( telecast_send_pid == 0 )
	{
		dup2( server_fd, 0 );
		dup2( server_fd, 1 );
		
		close( server_fd );
		
		dup2( fds[ 0 ], 3 );
		
		close( fds[ 0 ] );
		close( fds[ 1 ] );
		
		const char* argv[] = { TELECAST_SEND, "--feed=/dev/fd/3", raster_path, NULL };
		
		execvp( *argv, (char**) argv );
		
		const int saved_errno = errno;
		
		report_error( *argv, saved_errno );
		
		_exit( saved_errno == ENOENT ? 127 : 126 );
	}
	
	telecast_feed_fd = fds[ 1 ];
	
	close( fds[ 0 ] );
}

static
void launch_viewer( const char* raster_path )
{
	viewer_pid = fork();
	
	if ( viewer_pid < 0 )
	{
		report_error( "fork", errno );
		exit( 1 );
	}
	
	if ( viewer_pid == 0 )
	{
		const char* argv[] = { VIEWER, "-x", "1", raster_path, NULL };
		
		argv[ 2 ] = magnifier;
		
		execvp( *argv, (char**) argv );
		
		const int saved_errno = errno;
		
		report_error( *argv, saved_errno );
		
		_exit( saved_errno == ENOENT ? 127 : 126 );
	}
}

static
void launch_interactive( char* const* args )
{
	int pipe_fds  [ 2 ];
	int socket_fds[ 2 ];
	
	if ( feed_fd >= 0 )
	{
		int nok = pipe( pipe_fds );
		
		if ( nok )
		{
			report_error( "pipe", errno );
			exit( 1 );
		}
	}
	
	int nok = socketpair( PF_UNIX, SOCK_STREAM, 0, socket_fds );
	
	if ( nok )
	{
		report_error( "socketpair", errno );
		exit( 1 );
	}
	
	viewer_pid = fork();
	
	if ( viewer_pid < 0 )
	{
		report_error( "fork", errno );
		exit( 1 );
	}
	
	if ( viewer_pid == 0 )
	{
		dup2( socket_fds[ 1 ], 0 );
		dup2( socket_fds[ 1 ], 1 );
		
		close( socket_fds[ 0 ] );
		close( socket_fds[ 1 ] );
		
		const char* argv[] = { INTERACT, "--raster", raster_path, "-x", "1", NULL, NULL };
		
		if ( feed_fd >= 0 )
		{
			dup2( pipe_fds[ 0 ], 3 );
			
			close( pipe_fds[ 0 ] );
			close( pipe_fds[ 1 ] );
			
			argv[ 5 ] = "--feed=/dev/fd/3";
		}
		
		argv[ 4 ] = magnifier;
		
		execvp( *argv, (char**) argv );
		
		const int saved_errno = errno;
		
		report_error( *argv, saved_errno );
		
		_exit( saved_errno == ENOENT ? 127 : 126 );
	}
	
	if ( feed_fd >= 0 )
	{
		close( pipe_fds[ 0 ] );
		
		interact_feed_fd = pipe_fds[ 1 ];
	}
	
	author_pid = fork();
	
	if ( author_pid < 0 )
	{
		report_error( "fork", errno );
		exit( 1 );
	}
	
	if ( author_pid == 0 )
	{
		dup2( socket_fds[ 0 ], 0 );
		dup2( socket_fds[ 0 ], 1 );
		
		close( socket_fds[ 0 ] );
		close( socket_fds[ 1 ] );
		
		char* const* argv = args;
		
		execvp( *argv, (char**) argv );
		
		const int saved_errno = errno;
		
		report_error( *argv, saved_errno );
		
		_exit( saved_errno == ENOENT ? 127 : 126 );
	}
	
	close( socket_fds[ 0 ] );
	close( socket_fds[ 1 ] );
}

int main( int argc, char** argv )
{
	if ( argc == 0 )
	{
		return 0;
	}
	
	char* const* args = get_options( argv );
	
	int argn = argc - (args - argv);
	
	if ( raster_path == NULL )
	{
		write( STDERR_FILENO, STR_LEN( USAGE ) );
		return 2;
	}
	
	if ( argn > 0 )
	{
		// Only init the sync relay if we have an interactive author.
		init_raster( raster_path );
	}
	
	struct sigaction action;
	
	action.sa_handler = &sigchld;
	action.sa_mask    = 0;
	action.sa_flags   = 0;
	
	sigaction( SIGCHLD, &action, NULL );
	
	if ( char* telecast_addr = getenv( "TELECAST_ADDR" ) )
	{
		fprintf( stderr, "TELECAST_ADDR: %s\n", telecast_addr );
		
		if ( argn > 0 )
		{
			launch_raster_feed( raster_path );
		}
		
		int server_fd = connect_to_server( telecast_addr );
		
		launch_telecast_send( server_fd, raster_path );
		
		close( server_fd );
	}
	
	if ( argn == 0 )
	{
		// No author program: just display the raster image.
		launch_viewer( raster_path );
		
		int wait_status;
		pid_t child = wait( &wait_status );
	}
	else
	{
		launch_interactive( args );
	}
	
	if ( feed_fd >= 0 )
	{
		char c;
		
		while ( ! child_terminated  &&  read( feed_fd, &c, sizeof c ) > 0 )
		{
			if ( telecast_feed_fd >= 0 )  write( telecast_feed_fd, &c, sizeof c );
			if ( interact_feed_fd >= 0 )  write( interact_feed_fd, &c, sizeof c );
		}
	}
	
	int wait_status;
	
	pid_t child = wait( &wait_status );
	
	fprintf( stderr, "Exiting...\n" );
	
	if ( raster_feed_pid   )  kill( raster_feed_pid,   SIGTERM );
	if ( telecast_send_pid )  kill( telecast_send_pid, SIGTERM );
	if ( viewer_pid        )  kill( viewer_pid,        SIGTERM );
	if ( author_pid        )  kill( author_pid,        SIGTERM );
	
	child = wait( &wait_status );
	child = wait( &wait_status );
	child = wait( &wait_status );
	
	return 0;
}
