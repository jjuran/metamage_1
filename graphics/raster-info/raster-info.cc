/*
	raster-info.cc
	--------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// more-posix
#include "more/perror.hh"

// rasterlib
#include "raster/load.hh"


#define PROGRAM  "raster-info"


static const char* raster_models[] =
{
	"monochrome paint (1 == black)",
	"monochrome light (1 == white)",
	"palette",
	"RGB",
	"xRGB",
	"RGBx",
	"ARGB",
	"RGBA",
	"premultiplied ARGB",
	"premultiplied RGBA",
};

static raster::raster_load loaded_raster;


static inline
void report_error( const char* path, int err )
{
	more::perror( PROGRAM, path, err );
}

static
void open_raster( const char* path )
{
	int raster_fd = open( path, O_RDONLY );
	
	if ( raster_fd < 0 )
	{
		report_error( path, errno );
		
		exit( 1 );
	}
	
	using namespace raster;
	
	loaded_raster = load_raster( raster_fd );
	
	if ( loaded_raster.addr == NULL )
	{
		report_error( path, errno );
		
		exit( 1 );
	}
	
	close( raster_fd );
}

static
void print_info( const raster::raster_load& loaded_raster )
{
	using namespace raster;
	
	printf( "File size: %lu\n", (unsigned long) loaded_raster.size );
	
	const char* base = (char*) loaded_raster.addr;
	
	bool little_endian = (bool) *(uint16_t*) (base + loaded_raster.size - 4);
	
	printf( "Byte order: %s-endian\n", little_endian ? "little" : "big" );
	
	const raster_desc& desc = loaded_raster.meta->desc;
	
	const unsigned long min_stride = (desc.width * desc.weight + 7) / 8;
	
	char mb32_type[] = "'type'";
	
	const char* no_type = "(none)";
	const char* type    = no_type;
	
	if ( desc.magic )
	{
		memcpy( mb32_type + 1, &desc.magic, 4 );
		
		type = mb32_type;
	}
	
	printf( "Type: %s\n", type );
	printf( "Version: %u\n", desc.version );
	
	printf( "Image dimensions: %ux%u\n", desc.width, desc.height );
	printf( "Pixel weight (bit depth): %u\n", desc.weight );
	printf( "Bytes per row: %u (minimum: %lu)\n", desc.stride, min_stride );
	printf( "Image data bytes: %u\n", desc.height * desc.stride );
	printf( "Frame count: %u\n", 1 + desc.extra );
	
	if ( desc.model >= (uint8_t) Model_end_of_enumeration )
	{
		fprintf( stderr,
		         PROGRAM ": 'model' code %u is out of range\n", desc.model );
	}
	else
	{
		printf( "Pixel model: %s\n", raster_models[ desc.model ] );
	}
	
	const raster_note* note = &loaded_raster.meta->note;
	
	while ( exists( note ) )
	{
		memcpy( mb32_type + 1, &note->type, 4 );
		
		printf( "Note type: %s\n", mb32_type  );
		printf( "Note size: %u\n", note->size );
		
		note = next( note );
	}
}

int main( int argc, char** argv )
{
	if ( *argv++ == NULL )
	{
		return 0;
	}
	
	while ( const char* path = *argv++ )
	{
		open_raster( path );
		
		print_info( loaded_raster );
		
		close_raster( loaded_raster );
	}
	
	return 0;
}
