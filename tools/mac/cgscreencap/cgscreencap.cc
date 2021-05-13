/*
	cgscreencap.cc
	--------------
*/

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

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
#include "raster/skif.hh"


#define PROGRAM  "cgscreencap"

#define MISSING_OUTPUT  "stdout is a tty, either redirect or use -o"
#define NEEDS_OSX_10_6  "Mac OS X 10.6 or later required"

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
	
	CGImageRef image;
	
#ifdef MAC_OS_X_VERSION_10_6
	
	image = CGDisplayCreateImage( kCGDirectMainDisplay );
	
#else
	
	write( STDERR_FILENO, STR_LEN( PROGRAM ": " NEEDS_OSX_10_6 "\n" ) );
	exit( 2 );
	
#endif
	
	const size_t width  = CGImageGetWidth       ( image );
	const size_t height = CGImageGetHeight      ( image );
	const size_t stride = CGImageGetBytesPerRow ( image );
	const size_t weight = CGImageGetBitsPerPixel( image );
	
	const uint32_t image_size = height * stride;
	
	raster_desc desc =
	{
		raster::kSKIFFileType,
		0,
		width,
		height,
		stride,
		weight,
		Model_RGB,
		0,
		0,
		{ raster::xRGB },
	};
	
	CFDataRef data;
	
#ifdef MAC_OS_X_VERSION_10_5
	
	/*
		10.9's CGDataProvider.h states that CGDataProviderCopyData() is
		available in 10.3 and later, but it's absent in 10.4's headers.
	*/
	
	data = CGDataProviderCopyData( CGImageGetDataProvider( image ) );
	
#endif
	
	CGImageRelease( image );
	
	int fd = open( path, O_WRONLY | O_CREAT | O_TRUNC, 0666 );
	
	if ( fd < 0 )
	{
		report_error( path, errno );
		exit( 1 );
	}
	
	ssize_t n_written = write( fd, CFDataGetBytePtr( data ), image_size );
	
	if ( n_written < 0 )
	{
		report_error( path, errno );
		exit( 1 );
	}
	
	CFRelease( data );
	
	n_written = write( fd, &desc, sizeof desc );
	
	if ( n_written < 0 )
	{
		report_error( path, errno );
		exit( 1 );
	}
	
	uint32_t footer_size = sizeof desc + sizeof (uint32_t) * 2;
	
	const uint32_t disk_block_size = TARGET_API_MAC_CARBON ? 4096 : 512;
	const uint32_t k               = disk_block_size - 1;
	
	// Round up the total file size to a multiple of the disk block size.
	const uint32_t total_size = (image_size + footer_size + k) & ~k;
	
	footer_size = total_size - image_size;
	
	const off_t footer_addr = total_size - sizeof footer_size;
	
	const size_t code_len = sizeof (uint32_t);
	
	n_written = pwrite( fd, "SKIF", code_len, footer_addr - code_len );
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
