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

#define USAGE "Usage: " PROGRAM " [--fd N] server-command \"//\" client-command"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define WARN( s )  write( STDERR_FILENO, STR_LEN( s "\n" ) );

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
	WARN( USAGE );
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

static inline
void set_not_close_on_exec( int fd )
{
	CHECK_N( fcntl( fd, F_SETFD, 0 ) );
}

static
void launch( char** argv )
{
	(void) execvp( argv[ 0 ], argv );
	
fail:
	
	const int exit_code = errno == ENOENT ? 127 : 126;
	
	more::perror( PROGRAM, argv[ 0 ] );
	
	_exit( exit_code );
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

static
char** find_next_graft( char** args )
{
	while ( const char* p = *args++ )
	{
		if ( is_graft( p ) )
		{
			return --args;
		}
	}
	
	return NULL;
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
	
	int n_servers = 0;
	
	const int n_saveable_server_fds = 1024;
	
	int saved_server_fds[ n_saveable_server_fds ];
	
	int n_saved_server_fds = 0;
	int n_cloexec_fds = 0;
	
	char** next_graft = find_next_graft( args );
	
	while ( next_graft != NULL )
	{
		int chosen_fd = 0;
		
		if ( strcmp( *args, "--fd" ) == 0 )
		{
			++args;
			
			chosen_fd = atoi( *args++ );
			
			if ( chosen_fd == 2 )
			{
				usage();
				WARN( "       (where N is any file descriptor except 2)" );
				return 1;
			}
		}
		
		if ( is_dash( *args ) )
		{
			++args;
		}
		
		if ( next_graft == args )
		{
			return usage();
		}
		
		*next_graft++ = NULL;
		
		if ( *next_graft == NULL )
		{
			return usage();
		}
		
		const int input_fd  = chosen_fd ? chosen_fd : 6;
		const int output_fd = chosen_fd ? chosen_fd : 7;
		
		int server_fd = launch_server( args, input_fd, output_fd );
		
		++n_servers;
		
		args = next_graft;
		
		next_graft = find_next_graft( args );
		
		if ( next_graft == NULL )
		{
			n_cloexec_fds = n_saved_server_fds;
		}
		
		if ( chosen_fd )
		{
			dup2( server_fd, chosen_fd );
			
			saved_server_fds[ n_saved_server_fds++ ] = chosen_fd;
			
			if ( next_graft )
			{
				set_close_on_exec( chosen_fd );
			}
		}
		else
		{
			dup2( server_fd, 6 );
			dup2( server_fd, 7 );
			
			saved_server_fds[ n_saved_server_fds++ ] = 6;
			saved_server_fds[ n_saved_server_fds++ ] = 7;
			
			if ( next_graft )
			{
				set_close_on_exec( 6 );
				set_close_on_exec( 7 );
			}
		}
		
		close( server_fd );
	}
	
	for ( int i = 0;  i < n_cloexec_fds;  ++i )
	{
		set_not_close_on_exec( saved_server_fds[ i ] );
	}
	
	const pid_t client_pid = CHECK_N( vfork() );
	
	if ( client_pid == 0 )
	{
		launch( args );
	}
	
	for ( int i = 0;  i < n_saved_server_fds;  ++i )
	{
		close( saved_server_fds[ i ] );
	}
	
	int wait_status;
	
	do
	{
		pid_t pid = CHECK_N( wait( &wait_status ) );
	}
	while ( n_servers-- );
	
	return 0;
}
