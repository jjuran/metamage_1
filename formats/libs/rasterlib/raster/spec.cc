/*
	spec.cc
	-------
*/

#include "raster/spec.hh"

// rasterlib
#include "raster/raster.hh"


#define COUNT( v )  (sizeof v / sizeof *v)


namespace raster
{

typedef int (*parser_proc)( void* out, const char* text );

struct parse_step
{
	parser_proc  parser;
	void*        result;
};

static
int skip_x( void* null, const char* text )
{
	return *text == 'x';
}

static
int skip_colon( void* null, const char* text )
{
	return *text == ':';
}

static
int parse_depth_tag( char* result, const char* text )
{
	char c = *text;
	
	switch ( c )
	{
		case 'c':
		case 'g':
		case 'B':
		case 'L':
			*result = c;
			
			return 1;
	}
	
	return 0;
}

static
int parse_posint_u32( uint32_t* result, const char* text )
{
	const char* p = text;
	
	uint32_t x = 0;
	
	char c;
	
	if ( (uint8_t) ((c = *p++ - '1')) <= '9' - '1' )
	{
		x = c + 1;
		
		while ( (uint8_t) ((c = *p++ - '0')) <= '9' - '0' )
		{
			x = x * 10 + c;
		}
	}
	
	*result = x;
	
	return --p - text;
}

long parse_raster_spec( raster_spec& spec, const char* text )
{
	uint32_t bpp = 0;
	
	char depth_tag = '\0';
	
	const parse_step steps[] =
	{
		{ (parser_proc) &parse_posint_u32, &spec.width  },
		{ (parser_proc) &skip_x                         },
		{ (parser_proc) &parse_posint_u32, &spec.height },
		{ (parser_proc) &skip_colon                     },
		{ (parser_proc) &parse_posint_u32, &bpp         },
		{ (parser_proc) &parse_depth_tag,  &depth_tag   },
	};
	
	const char* p = text;
	
	for ( int i = 0;  i < COUNT( steps );  ++i )
	{
		const parse_step& step = steps[ i ];
		
		if ( int len = step.parser( step.result, p ) )
		{
			p += len;
		}
		else if ( i < 3 )
		{
			return 0;
		}
	}
	
	switch ( bpp )
	{
		case 1:
		case 2:
		case 4:
		case 8:
		case 16:
		case 32:
			break;
		
		default:
			return 0;
	}
	
	spec.weight = bpp;
	
	switch ( depth_tag )
	{
		case 'c':
			spec.chroma = Chroma_color;
			break;
		
		case 'g':
			spec.chroma = Chroma_grayscale;
			break;
		
		case 'B':
			spec.endian = Endian_big;
			break;
		
		case 'L':
			spec.endian = Endian_little;
			break;
		
		default:
			break;
	}
	
	return p - text;
}

}
