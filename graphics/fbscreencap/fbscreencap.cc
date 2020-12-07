/*
	fbscreencap.cc
	--------------
*/

// Android
#ifdef __ANDROID__
#include <sys/system_properties.h>
#endif

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// iota
#include "iota/endian.hh"

// more-posix
#include "more/perror.hh"

// raster
#include "raster/raster.hh"

// fbraster
#include "fb.hh"


#define PROGRAM  "fbscreencap"

#ifdef ANDROID
#define DEFAULT_FB_PATH  "/dev/graphics/fb0"
#else
#define DEFAULT_FB_PATH  "/dev/fb0"
#endif

#define MISSING_OUTPUT  "stdout is a tty, either redirect or use -o"

#define STR_LEN( s )  "" s, (sizeof s - 1)


static inline
void report_error( const char* path, uint32_t err )
{
	more::perror( PROGRAM, path, err );
}

static inline
bool is_rgba( const fb_var_screeninfo& var_info )
{
	return var_info.red.offset == 24;
}

static
void save_desktop_screenshot( const char* path )
{
	using raster::raster_desc;
	using raster::raster_model;
	using raster::Model_RGB;
	using raster::Model_ARGB;
	using raster::Model_RGBA;
	
	fb::handle fbh( DEFAULT_FB_PATH );
	
	fb_var_screeninfo var_info = get_var_screeninfo( fbh );
	fb_fix_screeninfo fix_info = get_fix_screeninfo( fbh );
	
	fb::map map( fbh );
	
	const size_t width  = var_info.xres;
	const size_t height = var_info.yres;
	const size_t stride = fix_info.line_length;
	const size_t weight = var_info.bits_per_pixel;
	
	/*
		Uee ARGB rather than xRGB, since Android may have a transparent screen
		when the camera is in use.  (Obviously this is not the case for 5/6/5,
		since there are no bits left over for alpha.)
		
		An older phone running Froyo has a 5/6/5 surface with the camera image.
		A Nexus S running Gingerbread has a transparent camera activity screen.
		A Nexus 4 running Jellybean doesn't -- the blank areas are 00 00 00 FF.
	*/
	
	const raster_model model = weight == 16 ? Model_RGB
	                                        : Model_ARGB;
	
	const uint32_t image_size = height * stride;
	
	raster_desc desc =
	{
		0,
		0,
		(uint32_t) width,
		(uint32_t) height,
		(uint32_t) stride,
		(uint8_t)  weight,
		(uint8_t)  model,
	};
	
	bool rgba = is_rgba( var_info );
	
#ifdef __ANDROID__
	
	/*
		An HP tablet running CyanogenMod (identifying as Android 4.3.1) reports
		var_info offsets of (red: 16, green: 8, blue: 0) -- i.e. it's claiming
		ARGB, but actually it's lying and is really RGBA (like all Android 4+,
		to my knowledge).
	*/
	
	if ( ! rgba )
	{
		char buffer[ PROP_VALUE_MAX + 1 ];
		int len = __system_property_get( "ro.build.version.release", buffer );
		
		if ( len >= 2  &&  (buffer[ 0 ] >= '4'  ||  buffer[ 1 ] != '.') )
		{
			rgba = true;
		}
	}
	
#endif
	
	if ( rgba )
	{
		desc.width  = iota::big_u32( desc.width  );
		desc.height = iota::big_u32( desc.height );
		desc.stride = iota::big_u32( desc.stride );
		
		desc.model = Model_RGBA;
	}
	
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
	
	if ( rgba )
	{
		footer_size = iota::big_u32( footer_size );
	}
	
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
