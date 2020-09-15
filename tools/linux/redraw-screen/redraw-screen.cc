/*
	redraw-screen.cc
	----------------
*/

// Linux
#include <linux/fb.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

// Standard C
#include <stdio.h>


#define PROGRAM  "redraw-screen"

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
	int fd = open( "/dev/fb0", O_WRONLY );
	
	check_nok( "/dev/fb0", fd < 0 );
	
	fb_var_screeninfo var_info;
	check_nok( PROGRAM ": FBIOGET_VSCREENINFO",
	           ioctl( fd, FBIOGET_VSCREENINFO, &var_info ) );
	
	var_info.activate = FB_ACTIVATE_FORCE;
	
	check_nok( PROGRAM ": FBIOPUT_VSCREENINFO",
	           ioctl( fd, FBIOPUT_VSCREENINFO, &var_info ) );
	
	return 0;
}
