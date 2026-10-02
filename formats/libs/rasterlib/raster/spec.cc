/*
	spec.cc
	-------
*/

#include "raster/spec.hh"


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
	const parse_step steps[] =
	{
		{ (parser_proc) &parse_posint_u32, &spec.width  },
		{ (parser_proc) &skip_x                         },
		{ (parser_proc) &parse_posint_u32, &spec.height },
	};
	
	const char* p = text;
	
	for ( int i = 0;  i < COUNT( steps );  ++i )
	{
		const parse_step& step = steps[ i ];
		
		if ( int len = step.parser( step.result, p ) )
		{
			p += len;
		}
		else
		{
			return 0;
		}
	}
	
	return p - text;
}

}
