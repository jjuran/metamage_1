/*
	kdmode.cc
	---------
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
#include <string.h>


static
void check_nok( const char* s, int nok )
{
	if ( nok )
	{
		perror( s );
		
		_exit( 1 );
	}
}

static
void signal_handler( int )
{
}

int main( int argc, char** argv )
{
	int tty0_fd = open( "/dev/tty0", O_WRONLY );
	
	check_nok( "/dev/tty0", tty0_fd < 0 );
	
	long mode;
	check_nok( "KDGETMODE", ioctl( tty0_fd, KDGETMODE, &mode ) );
	
	/*
		KD_TEXT      0x00
		KD_GRAPHICS  0x01
	*/
	
	const char* modes[] = { "text", "graphics" };
	
	if ( argc <= 1 )
	{
		if ( (unsigned long) mode > sizeof modes / sizeof modes[ 0 ] )
		{
			fprintf( stderr, "unknown mode: %ld\n", mode );
			return 1;
		}
		
		printf( "%s\n", modes[ mode ] );
		
		return 0;
	}
	
	const char* mode_arg = argv[ 1 ];
	
	long new_mode = strcmp( mode_arg, modes[ 0 ] ) == 0 ? 0
	              : strcmp( mode_arg, modes[ 1 ] ) == 0 ? 1
	              :                                      -1;
	
	if ( new_mode < 0 )
	{
		fprintf( stderr, "invalid mode '%s'\n", mode_arg );
		return 2;
	}
	
	signal( SIGHUP,  &signal_handler );
	signal( SIGINT,  &signal_handler );
	signal( SIGTERM, &signal_handler );
	
	check_nok( "KDSETMODE", ioctl( tty0_fd, KDSETMODE, new_mode ) );
	
	write( STDOUT_FILENO, "\n", 1 );
	
	char dummy;
	read( STDIN_FILENO, &dummy, sizeof dummy );
	
	check_nok( "KDSETMODE", ioctl( tty0_fd, KDSETMODE, mode ) );
	
	return 0;
}
