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
	Opt_output = 'o',
	Opt_length = 'z',  // siZe, or reZolution
};

static command::option options[] =
{
	{ "output-file", Opt_output, Param_required },
	{ "edge-length", Opt_length, Param_required },
	{ NULL }
};

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
	
	write_PNG_image( c, path );
	
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
