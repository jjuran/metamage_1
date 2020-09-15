/*
	pausecursor.cc
	--------------
*/

// Linux
#include <linux/kd.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

// Standard C
#include <signal.h>
#include <stdio.h>


static void check_nok( const char* s, int nok )
{
	if ( nok )
	{
		perror( s );
		
		_exit( 1 );
	}
}

static void sighandler( int )
{
	/*
		Do nothing; just let read() restart.
		
		The reason we restart read() is because we want to make sure that
		the other process has a chance to restore the framebuffer before
		we reenable the cursor blinking.
	*/
}

int main( int argc, char** argv )
{
	struct sigaction action = { 0 };
	action.sa_handler = &sighandler;
	action.sa_flags = SA_RESTART;
	
	sigaction( SIGHUP, &action, NULL );
	sigaction( SIGINT, &action, NULL );
	sigaction( SIGTERM, &action, NULL );
	
	int tty1_fd = open( "/dev/tty1", O_WRONLY );
	
	if ( tty1_fd < 0 )
	{
		return 1;
	}
	
	check_nok( "KD_GRAPHICS", ioctl( tty1_fd, KDSETMODE, KD_GRAPHICS ) );
	
	char buffer[ 1 ];
	read( STDIN_FILENO, buffer, 1 );
	
	check_nok( "KD_TEXT", ioctl( tty1_fd, KDSETMODE, KD_TEXT ) );
	
	return 0;
}
