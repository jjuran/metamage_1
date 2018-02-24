/*
	main.cc
	-------
*/

// POSIX
#include <unistd.h>

// must
#include "must/write.h"

// command
#include "command/get_option.hh"

// plus
#include "plus/var_string.hh"

// bignum
#include "bignum/integer_hex.hh"

// vlib
#include "vlib/interpret.hh"
#include "vlib/string-utils.hh"
#include "vlib/tracker.hh"


using namespace vlib;


enum
{
	Opt_no_GC_culling = 'G',
	Opt_hex_output    = 'x',
};

static command::option options[] =
{
	{ "hex",   Opt_hex_output    },
	{ "no-gc", Opt_no_GC_culling },
	{ NULL },
};

static bool hex_output = false;

static plus::string stringify( const Value& v )
{
	if ( hex_output  &&  v.type() == Value_number )
	{
		return hex( v.number() );
	}
	
	return rep( v );
}

static void reproduce( const Value& v )
{
	if ( v.type() == Value_nothing )
	{
		return;
	}
	
	plus::var_string s = stringify( v ).move();
	
	s += "\n";
	
	must_write( STDOUT_FILENO, s.data(), s.size() );
}


static char* const* get_options( char** argv )
{
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		switch ( opt )
		{
			case Opt_hex_output:
				hex_output = true;
				break;
			
			case Opt_no_GC_culling:
				disable_gc_culling();
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

int main( int argc, char** argv )
{
	if ( argc == 0 )
	{
		return 0;
	}
	
	char* const* args = get_options( argv );
	
	const int argn = argc - (args - argv);
	
	if ( argn == 0 )
	{
		return 0;
	}
	
	int i = 0;
	
	do
	{
		const char* expr = args[ i ];
		
		const Value result = interpret( expr );
		
		reproduce( result );
	}
	while ( ++i < argn );
	
	return 0;
}
