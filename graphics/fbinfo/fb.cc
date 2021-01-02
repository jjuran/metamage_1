/*
	fb.cc
	-----
*/

#include "fb.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

// Standard C
#include <errno.h>


namespace fb
{
	
	handle::handle( const char* path ) : fd( open( path, O_RDWR ) )
	{
		if ( fd < 0 )
		{
			throw errno;
		}
	}
	
	handle::~handle()
	{
		close( fd );
	}
	
	
	fb_fix_screeninfo get_fix_screeninfo( const handle& h )
	{
		fb_fix_screeninfo result;
		
		int fail = ioctl( h.get_fd(), FBIOGET_FSCREENINFO, &result );
		
		if ( fail )
		{
			throw errno;
		}
		
		return result;
	}
	
	fb_var_screeninfo get_var_screeninfo( const handle& h )
	{
		fb_var_screeninfo result;
		
		int fail = ioctl( h.get_fd(), FBIOGET_VSCREENINFO, &result );
		
		if ( fail )
		{
			throw errno;
		}
		
		return result;
	}
	
}
