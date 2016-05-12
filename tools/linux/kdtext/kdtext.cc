/*
	kdtext.cc
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
	
	check_nok( "KD_TEXT", ioctl( tty0_fd, KDSETMODE, KD_TEXT ) );
	
	return 0;
}
