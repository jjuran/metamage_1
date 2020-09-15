/*
	unblank.cc
	----------
*/

// Linux
#include <linux/fb.h>

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
	int fb0 = open( "/dev/fb0", O_WRONLY );
	
	check_nok( "/dev/fb0", fb0 < 0 );
	
	check_nok( "FB_BLANK_UNBLANK", ioctl( fb0, FBIOBLANK, FB_BLANK_UNBLANK ) );
	
	return 0;
}
