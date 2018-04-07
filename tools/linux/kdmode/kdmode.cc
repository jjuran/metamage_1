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
#include <stdio.h>


static
void check_nok( const char* s, int nok )
{
	if ( nok )
	{
		perror( s );
		
		_exit( 1 );
	}
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
	
	if ( (unsigned long) mode > sizeof modes / sizeof modes[ 0 ] )
	{
		check_nok( "unknown mode", -1 );
	}
	
	printf( "%s\n", modes[ mode ] );
	
	return 0;
}
