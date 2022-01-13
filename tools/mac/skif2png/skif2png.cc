/*
	skif2png.cc
	-----------
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
#include <stdio.h>
#include <string.h>

// more-posix
#include "more/perror.hh"

// rasterlib
#include "raster/load.hh"

// CGSKIF
#include "CGSKIF.hh"


#define PROGRAM  "skif2png"

#define MISSING_INPUT   "input file required"
#define MISSING_OUTPUT  "stdout is a tty, either redirect or use -o"

#define STR_LEN( s )  "" s, (sizeof s - 1)


using namespace raster;


static inline
void report_error( const char* path, uint32_t err )
{
	more::perror( PROGRAM, path, err );
}

static
void write_image_as_type( CGImageRef image, const char* path, CFStringRef type )
{
	CFURLRef url;
	url = CFURLCreateFromFileSystemRepresentation( NULL,
	                                               (const uint8_t*) path,
	                                               strlen( path ),
	                                               false );
	
	if ( url == NULL )
	{
		fprintf( stderr, "CFURLCreateFromFileSystemRepresentation() returned NULL\n" );
		exit( 1 );
	}
	
	CGImageDestinationRef dest;
	dest = CGImageDestinationCreateWithURL( url, type, 1, NULL );
	
	CFRelease( url );
	
	if ( dest == NULL )
	{
		fprintf( stderr, "CGImageDestinationCreateWithURL() returned NULL\n" );
		exit( 1 );
	}
	
	CGImageDestinationAddImage( dest, image, NULL );
	
	CGImageDestinationFinalize( dest );
	
	CFRelease( dest );
}

static
void raster_to_PNG( const raster_load& raster, const char* path )
{
	CGImageRef image = CGSKIFCreateImageFromRaster( raster );
	
	write_image_as_type( image, path, kUTTypePNG );
	
	CGImageRelease( image );
}

int main( int argc, char** argv )
{
	const char* output_path = "/dev/fd/1";
	
	char** args = argv + 1;
	
	if ( argc >= 3  &&  strcmp( *args, "-o" ) == 0 )
	{
		++args;
		output_path = *args++;
	}
	else if ( isatty( STDOUT_FILENO ) )
	{
		write( STDERR_FILENO, STR_LEN( PROGRAM ": " MISSING_OUTPUT "\n" ) );
		return 2;
	}
	
	const char* src = *args;
	
	if ( src == NULL )
	{
		write( STDERR_FILENO, STR_LEN( PROGRAM ": " MISSING_INPUT "\n" ) );
		return 2;
	}
	
	int fd = open( src, O_RDONLY );
	
	if ( fd < 0 )
	{
		report_error( src, errno );
		return 1;
	}
	
	raster_load raster = load_raster( fd );
	
	if ( raster.addr == NULL )
	{
		report_error( src, errno );
		return 1;
	}
	
	raster_to_PNG( raster, output_path );
	
	close( fd );
	
	return 0;
}
