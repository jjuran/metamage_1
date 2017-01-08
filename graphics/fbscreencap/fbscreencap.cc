/*
	fbscreencap.cc
	--------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// more-posix
#include "more/perror.hh"

// raster
#include "raster/raster.hh"

// fbraster
#include "fb.hh"


#define PROGRAM  "fbscreencap"

#define DEFAULT_FB_PATH  "/dev/fb0"

#define MISSING_OUTPUT  "stdout is a tty, either redirect or use -o"

#define STR_LEN( s )  "" s, (sizeof s - 1)


static inline
void report_error( const char* path, uint32_t err )
{
	more::perror( PROGRAM, path, err );
}

static
void save_desktop_screenshot( const char* path )
{
	using raster::raster_desc;
	using raster::Model_RGB;
	
	fb::handle fbh( DEFAULT_FB_PATH );
	
	fb_var_screeninfo var_info = get_var_screeninfo( fbh );
	fb_fix_screeninfo fix_info = get_fix_screeninfo( fbh );
	
	fb::map map( fbh );
	
	const size_t width  = var_info.xres;
	const size_t height = var_info.yres;
	const size_t stride = fix_info.line_length;
	const size_t weight = var_info.bits_per_pixel;
	
	const uint32_t image_size = height * stride;
	
	raster_desc desc =
	{
		0,
		0,
		width,
		height,
		stride,
		weight,
		Model_RGB,
	};
	
	int fd = open( path, O_WRONLY | O_CREAT | O_TRUNC, 0666 );
	
	if ( fd < 0 )
	{
		report_error( path, errno );
		exit( 1 );
	}
	
	ssize_t n_written = write( fd, map.get_base(), image_size );
	
	if ( n_written < 0 )
	{
		report_error( path, errno );
		exit( 1 );
	}
	
	n_written = write( fd, &desc, sizeof desc );
	
	if ( n_written < 0 )
	{
		report_error( path, errno );
		exit( 1 );
	}
	
	uint32_t footer_size = sizeof desc + sizeof (uint32_t);
	
	const uint32_t disk_block_size = 4096;
	const uint32_t k               = disk_block_size - 1;
	
	// Round up the total file size to a multiple of the disk block size.
	const uint32_t total_size = (image_size + footer_size + k) & ~k;
	
	footer_size = total_size - image_size;
	
	const off_t footer_addr = total_size - sizeof footer_size;
	
	n_written = pwrite( fd, &footer_size, sizeof footer_size, footer_addr );
	
	if ( n_written < 0 )
	{
		report_error( path, errno );
		exit( 1 );
	}
	
	int nok = close( fd );
	
	if ( nok < 0 )
	{
		report_error( path, errno );
		exit( 1 );
	}
}

int main( int argc, char** argv )
{
	const char* filename = "/dev/fd/1";
	
	if ( argc >= 3  &&  strcmp( argv[ 1 ], "-o" ) == 0 )
	{
		filename = argv[ 2 ];
	}
	else if ( isatty( STDOUT_FILENO ) )
	{
		write( STDERR_FILENO, STR_LEN( PROGRAM ": " MISSING_OUTPUT "\n" ) );
		return 2;
	}
	
	save_desktop_screenshot( filename );
	
	return 0;
}
