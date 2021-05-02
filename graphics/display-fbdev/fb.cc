/*
	fb.cc
	-----
*/

#include "fb.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

// Standard C
#include <errno.h>
#include <stdio.h>


namespace fb
{
	
	handle::handle( const char* path ) : fd( open( path, O_RDWR ) )
	{
		if ( fd < 0 )
		{
			perror( path );
			throw errno;
		}
	}
	
	handle::~handle()
	{
		::close( fd );
	}
	
	
	map::map( const handle& h )
	{
		fb_fix_screeninfo fix_info = get_fix_screeninfo( h );
		
		size   = fix_info.smem_len;
		stride = fix_info.line_length;
		
		base = mmap( 0, size, PROT_READ|PROT_WRITE, MAP_SHARED, h.get_fd(), 0 );
		
		if ( base == NULL )
		{
			perror( "display-fbdev: mmap()" );
			throw errno;
		}
	}
	
	map::~map()
	{
		::munmap( base, size );
	}
	
	
	fb_fix_screeninfo get_fix_screeninfo( const handle& h )
	{
		fb_fix_screeninfo result;
		
		int fail = ioctl( h.get_fd(), FBIOGET_FSCREENINFO, &result );
		
		if ( fail )
		{
			perror( "display-fbdev: FBIOGET_FSCREENINFO" );
			throw errno;
		}
		
		ioctl( h.get_fd(), FBIOBLANK, FB_BLANK_UNBLANK );
		
		return result;
	}
	
	fb_var_screeninfo get_var_screeninfo( const handle& h )
	{
		fb_var_screeninfo result;
		
		int fail = ioctl( h.get_fd(), FBIOGET_VSCREENINFO, &result );
		
		if ( fail )
		{
			perror( "display-fbdev: FBIOGET_VSCREENINFO" );
			throw errno;
		}
		
		return result;
	}
	
	void set_var_screeninfo( int fd, const fb_var_screeninfo& info )
	{
		int fail = ioctl( fd, FBIOPUT_VSCREENINFO, &info );
		
		if ( fail )
		{
			perror( "display-fbdev: FBIOPUT_VSCREENINFO" );
			throw errno;
		}
	}
	
}
