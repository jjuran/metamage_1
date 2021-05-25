/*
	teletype.cc
	-----------
*/

// Standard C
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// POSIX
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/ttycom.h>
#include <sys/wait.h>
#include <utime.h>

// iota
#include "iota/freestore_free.hh"

// Relix
#include "relix/fork_and_exit.h"


#pragma exceptions off


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define CHECK_NEG( result )   if ( (result) <  0    )  return 1; else
#define CHECK_NULL( result )  if ( (result) == NULL )  return 1; else


static
int splat( const char* path, const char* buffer, size_t length )
{
	int fd = open( path, O_WRONLY | O_TRUNC );
	
	ssize_t n = write( fd, buffer, length );
	
	close( fd );
	
	return (fd >= 0  &&  n == length) - 1;
}

int main( int argc, char const *const argv[] )
{
	CHECK_NEG( chdir( "/gui/new/port" ) );
	
	char port_buffer[ sizeof "/gui/port/abcd3210" ];
	
	CHECK_NULL( getcwd( port_buffer, sizeof port_buffer ) );
	
	CHECK_NEG( setenv( "PORT", port_buffer, true ) );
	
	const short width  = 2 * 4 +  6 * 80 + 15;
	const short height = 2 * 4 + 11 * 24;
	
	splat( "size", STR_LEN( "503x272" "\n" ) );
	
	splat( ".~title", STR_LEN( "teletype" ) );
	
	CHECK_NEG( utime( "window", NULL ) );
	
	splat( "w/text-font", STR_LEN( "4" "\n" ) );
	splat( "w/text-size", STR_LEN( "9" "\n" ) );
	
	CHECK_NEG( link( "/gui/new/scrollframe", "view"     ) );
	CHECK_NEG( link( "/gui/new/frame",       "v/view"   ) );
	CHECK_NEG( link( "/gui/new/console",     "v/v/view" ) );
	
	CHECK_NEG( symlink( "v/v", "v/target" ) );
	
	splat( "v/vertical", STR_LEN( "1" "\n" ) );
	splat( "v/v/padding", STR_LEN( "4" "\n" ) );
	
	CHECK_NEG( rename( "v/v/v/tty", "tty" ) );
	
	int tty = open( "tty", O_RDWR );
	
	CHECK_NEG( tty );
	
	CHECK_NEG( dup2( tty, STDIN_FILENO  ) );
	CHECK_NEG( dup2( tty, STDOUT_FILENO ) );
	CHECK_NEG( dup2( tty, STDERR_FILENO ) );
	
	if ( tty > STDERR_FILENO )
	{
		close( tty );
	}
	
	if ( getsid( 0 ) != 1 )
	{
		// Ensure that the caller doesn't wait for us
		// (Also ensures we are not a process group leader)
		fork_and_exit( 0 );
	}
	
	pid_t sid = setsid();  // New session
	
	CHECK_NEG( sid );
	
	// Restore default SIGHUP handling in case we were invoked from daemonize
	
	struct sigaction action = { SIG_DFL, 0, 0 };
	
	sigaction( SIGHUP, &action, NULL );
	
	CHECK_NEG( ioctl( STDIN_FILENO, TIOCSCTTY, NULL ) );  // Reattach to tty
	
	if ( const char* name = ttyname( STDIN_FILENO ) )
	{
		splat( ".~title", name, strlen( name ) );
	}
	
	pid_t pid = vfork();
	
	if ( pid == 0 )
	{
		chdir( "/" );
		
		char* login_argv[] = { "/bin/login", NULL };
		
		const bool use_login = argc < 2;
		
		const char* const* next_argv = use_login ? login_argv
		                                         : argv + 1;
		
		execv( next_argv[0], (char**) next_argv );
		
		_exit( 1 );
	}
	
	int wait_status;
	
	CHECK_NEG( wait( &wait_status ) );
	
	// If the child exited unsuccessfully and the terminal is still connected,
	// keep the window around.
	if ( wait_status != 0 )
	{
		// Reset ourselves as the foreground process group of the terminal,
		// so we'll get SIGHUP when the window is closed.
		int pgrpset = tcsetpgrp( STDIN_FILENO, sid );
		
		splat( ".~title", STR_LEN( "(Done)" ) );
		
		// Loop so resizing the window doesn't kill us (via SIGWINCH).
		while ( true )
		{
			pause();
		}
	}
	
	return 0;
}
