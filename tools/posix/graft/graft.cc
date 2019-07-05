/*
	graft.cc
	--------
*/

// Standard C
#include <errno.h>
#include <stdlib.h>
#include <string.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>

// more-posix
#include "more/perror.hh"


#pragma exceptions off


#define PROGRAM  "graft"

#define USAGE "Usage: " PROGRAM " [--fd N] server-command \"//\" client-command\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)


#define CHECK_N( call )  check_n( call, #call )

static inline
int check_n( int result, const char* code )
{
	if ( result < 0 )
	{
		more::perror( PROGRAM, code );
		
		_exit( 1 );
	}
	
	return result;
}

static
int usage()
{
	write( STDERR_FILENO, STR_LEN( USAGE ) );
	return 1;
}

static inline
bool is_dash( const char* p )
{
	return *p++ == '-'  &&
	       *p++ == '-'  &&
	       *p   == '\0';
}

static inline
bool is_graft( const char* p )
{
	return *p++ == '/'  &&
	       *p++ == '/'  &&
	       *p   == '\0';
}

static inline
void set_close_on_exec( int fd )
{
	CHECK_N( fcntl( fd, F_SETFD, FD_CLOEXEC ) );
}

static
void launch( int local, int in, int out, char** argv )
{
	if ( dup2( local, in  ) < 0 )  goto fail;
	if ( dup2( local, out ) < 0 )  goto fail;
	
	close( local );
	
	(void) execvp( argv[ 0 ], argv );
	
fail:
	
	const int exit_code = errno == ENOENT ? 127 : 126;
	
	more::perror( PROGRAM, argv[ 0 ] );
	
	_exit( exit_code );
}

static
int launch_server( char** server_args, int input_fd, int output_fd )
{
	// Invoke subprocesses
	// -------------------
	
	/*
		Reserve I/O fds so neither becomes one of our sockets.
		Otherwise, we might clobber them when launching the client.
	*/
	
	CHECK_N( dup2( 2, input_fd  ) );
	CHECK_N( dup2( 2, output_fd ) );
	
	set_close_on_exec( input_fd  );
	set_close_on_exec( output_fd );
	
	int fds[ 2 ];
	
	CHECK_N( socketpair( PF_UNIX, SOCK_STREAM, 0, fds ) );
	
	set_close_on_exec( fds[ 1 ] );
	
	const pid_t server_pid = CHECK_N( vfork() );
	
	if ( server_pid == 0 )
	{
		launch( fds[ 0 ], 0, 1, server_args );
	}
	
	close( fds[ 0 ] );
	
	return fds[ 1 ];
}

int main( int argc, char** argv )
{
	// Process arguments
	// -----------------
	
	char** args = argv + 1;
	
	int argn = argc - 1;
	
	if ( argn < 3 )
	{
		return usage();
	}
	
	int chosen_fd = 0;
	
	if ( strcmp( *args, "--fd" ) == 0 )
	{
		++args;
		
		chosen_fd = atoi( *args++ );
		
		if ( chosen_fd < 3 )
		{
			return usage();
		}
	}
	
	char** server_args = args;
	
	if ( is_dash( *server_args ) )
	{
		++server_args;
	}
	
	char** graft_arg = server_args;
	
	while ( *graft_arg  &&  ! is_graft( *graft_arg ) )
	{
		++graft_arg;
	}
	
	if ( *graft_arg == NULL  ||  graft_arg == server_args )
	{
		return usage();
	}
	
	*graft_arg = NULL;
	
	char** client_args = graft_arg + 1;
	
	if ( *client_args == NULL )
	{
		return usage();
	}
	
	const int input_fd  = chosen_fd ? chosen_fd : 6;
	const int output_fd = chosen_fd ? chosen_fd : 7;
	
	int server_fd = launch_server( server_args, input_fd, output_fd );
	
	const pid_t client_pid = CHECK_N( vfork() );
	
	if ( client_pid == 0 )
	{
		launch( server_fd, input_fd, output_fd, client_args );
	}
	
	close( server_fd );
	
	int wait_status;
	
	pid_t pid_a = CHECK_N( wait( &wait_status ) );
	pid_t pid_b = CHECK_N( wait( &wait_status ) );
	
	return 0;
}
