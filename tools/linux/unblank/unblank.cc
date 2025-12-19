/*
	unblank.cc
	----------
*/

// Linux
#include <linux/fb.h>

// POSIX
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

// Standard C
#include <stdio.h>
#include <stdlib.h>


static inline
const char* get_framebuffer()
{
	const char* framebuffer = getenv( "FRAMEBUFFER" );
	
	return framebuffer ? framebuffer : "/dev/fb0";
}

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
	const char* framebuffer = get_framebuffer();
	
	int fb0 = open( framebuffer, O_WRONLY );
	
	check_nok( framebuffer, fb0 < 0 );
	
	check_nok( "FB_BLANK_UNBLANK", ioctl( fb0, FBIOBLANK, FB_BLANK_UNBLANK ) );
	
	return 0;
}
