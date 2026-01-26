/*
	dec2str.cc
	----------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FP__
#include <fp.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"

// gear
#include "gear/parse_decimal.hh"

// command
#include "command/get_option.hh"


using namespace command::constants;

enum
{
	Option_n_digits = 'd',
	Option_floatexp = 'e',
	Option_fixedexp = 'x',
};

static command::option options[] =
{
	{ "", Option_n_digits, Param_required },
	{ "", Option_floatexp, Param_required },
	{ "", Option_fixedexp, Param_required },
	
	{ NULL }
};

static int n_digits = 3;
static int exponent = 0;
static int style    = 1;

static
char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		int x = gear::parse_decimal( command::global_result.param );
		
		switch ( opt )
		{
			case Option_n_digits:
				n_digits = x;
				break;
			
			case Option_floatexp:
			case Option_fixedexp:
				exponent = x;
				style    = ~opt & 0x1;  // true for 'x', false for 'e'
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

static inline
size_t min( size_t a, size_t b )
{
	return b < a ? b : a;
}

static
void report( const char* s )
{
	decimal dec  = {};
	decform form = {};
	
	form.style  = style;
	form.digits = n_digits;
	
	dec.exp = exponent;
	
	if ( *s == '-' )
	{
		++s;
		
		dec.sgn = 1;
	}
	
	size_t n = min( strlen( s ), SIGDIGLEN );
	
	dec.sig.length = n;
	
	mempcpy( dec.sig.text, s, n );
	
	char buffer[ 256 ];
	
	dec2str( &form, &dec, buffer );
	
	n = strlen( buffer );
	
	buffer[ n++ ] = '\n';
	
	write( STDOUT_FILENO, buffer, n );
}

int main( int argc, char** argv )
{
	char *const *args = get_options( argv );
	
	const int argn = argc - (args - argv);
	
	if ( const char* s = args[ 0 ] )
	{
		report( s );
		
		return 0;
	}
	
	return 2;
}
