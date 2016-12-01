/*
	icongen.cc
	----------
*/

#include "icongen/icongen.hh"

// Standard C
#include <stdio.h>

// gear
#include "gear/parse_decimal.hh"

// command
#include "command/get_option.hh"

// icongen
#include "icongen/utils.hh"


#define PROGRAM  "icongen"

using namespace command::constants;


enum
{
	Opt_JP2    = 'J',
	Opt_PNG    = 'P',
	Opt_output = 'o',
	Opt_RLE_24 = 't',  // thumbnail 24-bit RLE-compressed planar data
	Opt_mask_8 = 'k',  // thumbnail 8-bit mask
	Opt_length = 'z',  // siZe, or reZolution
};

static command::option options[] =
{
	{ "png",         Opt_PNG                    },
	{ "jp2",         Opt_JP2                    },
	{ "jp2k",        Opt_JP2                    },
	{ "thumbnail",   Opt_RLE_24                 },
	{ "mask",        Opt_mask_8                 },
	{ "output-file", Opt_output, Param_required },
	{ "edge-length", Opt_length, Param_required },
	{ NULL }
};

enum Image_mode
{
	Image_PNG,
	Image_JP2K,
	Image_thumbnail,
	Image_mask,
};

static Image_mode image_mode;

const char* output_path = NULL;
const char* edge_length = NULL;

static
char* const* get_options( char** argv )
{
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		switch ( opt )
		{
			case Opt_output:
				output_path = command::global_result.param;
				break;
			
			case Opt_PNG:
				image_mode = Image_PNG;
				break;
			
			case Opt_JP2:
				image_mode = Image_JP2K;
				break;
			
			case Opt_RLE_24:
				image_mode  = Image_thumbnail;
				edge_length = "128";
				break;
			
			case Opt_mask_8:
				image_mode  = Image_mask;
				edge_length = "128";
				break;
			
			case Opt_length:
				edge_length = command::global_result.param;
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

static
int draw_icon( drawer draw, size_t length, const char* path )
{
	const size_t width  = length;
	const size_t height = length;
	
	CGContextRef c = create_bitmap_context( length );
	
	draw( c, width, height );
	
	switch ( image_mode )
	{
		case Image_PNG:
			write_PNG_image( c, path );
			break;
		
		case Image_JP2K:
			write_JPEG_2000( c, path );
			break;
		
		case Image_thumbnail:
			write_thumbnail( c, path );
			break;
		
		case Image_mask:
			write_mask( c, path );
			break;
		
		default:
			break;
	}
	
	void* p = CGBitmapContextGetData( c );
	
	free( p );
	
	CGContextRelease( c );
	
	return 0;
}

int icongen_main( int argc, char** argv, drawer draw )
{
	if ( argc == 0 )
	{
		return 0;
	}
	
	char* const* args = get_options( argv );
	
	const int argn = argc - (args - argv);
	
	if ( output_path == NULL )
	{
		fprintf( stderr, PROGRAM ": -o switch is required\n" );
		return 2;
	}
	
	size_t length = 512;
	
	if ( edge_length != NULL )
	{
		length = gear::parse_unsigned_decimal( edge_length );
		
		if ( length < 32 )
		{
			fprintf( stderr, PROGRAM ": length must be at least 32, not %lu\n", length );
			return 2;
		}
		
		if ( length > 1024 )
		{
			fprintf( stderr, PROGRAM ": length must be at most 1024, not %lu\n", length );
			return 2;
		}
	}
	
	return draw_icon( draw, length, output_path );
}
