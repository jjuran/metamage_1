/*	=========
 *	jgetty.cc
 *	=========
 */

// POSIX
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <unistd.h>

// Lamp
#include "lamp/winio.h"


int main( int argc, char const *const argv[] )
{
	int console = open( "/dev/new/console", O_RDWR, 0 );
	
	if ( console == -1 )
	{
		return 1;
	}
	
	dup2( console, STDIN_FILENO  );
	dup2( console, STDOUT_FILENO );
	dup2( console, STDERR_FILENO );
	
	if ( console > STDERR_FILENO )
	{
		close( console );
	}
	
	int sid = setsid();  // New session
	
	if ( sid < 0 )
	{
		return 2;
	}
	
	int result = ioctl( STDIN_FILENO, TIOCSCTTY, NULL );  // Reattach to terminal
	
	if ( result < 0 )
	{
		return 3;
	}
	
	pid_t pid = vfork();
	
	if ( pid < 0 )
	{
		return 4;
	}
	
	if ( pid == 0 )
	{
		char* login_argv[] = { "/bin/login", NULL };
		
		(void) execv( login_argv[0], login_argv );
		
		_exit( 127 );
	}
	
	int wait_status = -1;
	
	int waited = wait( &wait_status );
	
	if ( waited < 0 )
	{
		return 5;
	}
	
	// If the child exited unsuccessfully and the terminal is still connected,
	// keep the window around.
	if ( wait_status != 0  &&  write( STDOUT_FILENO, "", 0 ) == 0 )
	{
		// Reset ourselves as the foreground process group of the terminal,
		// so we'll get SIGHUP when the window is closed.
		int pgrpset = tcsetpgrp( STDIN_FILENO, sid );
		
		result = ioctl( STDIN_FILENO, WIOCSTITLE, "(Done)" );
		
		// Loop so resizing the window doesn't kill us (via SIGWINCH).
		while ( true )
		{
			pause();
		}
	}
	
	return 0;
}

