/*
	redraw-screen.cc
	----------------
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


#define PROGRAM  "redraw-screen"


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
	
	int fd = open( framebuffer, O_WRONLY );
	
	check_nok( framebuffer, fd < 0 );
	
	fb_var_screeninfo var_info;
	check_nok( PROGRAM ": FBIOGET_VSCREENINFO",
	           ioctl( fd, FBIOGET_VSCREENINFO, &var_info ) );
	
	var_info.activate = FB_ACTIVATE_FORCE;
	
	check_nok( PROGRAM ": FBIOPUT_VSCREENINFO",
	           ioctl( fd, FBIOPUT_VSCREENINFO, &var_info ) );
	
	return 0;
}
