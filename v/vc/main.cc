/*
	main.cc
	-------
*/

// POSIX
#include <unistd.h>

// Standard C++
#include <new>

// must
#include "must/write.h"

// command
#include "command/get_option.hh"

// plus
#include "plus/integer_hex.hh"
#include "plus/string/concat.hh"

// vlib
#include "vlib/calc.hh"
#include "vlib/init.hh"
#include "vlib/library.hh"
#include "vlib/parse.hh"
#include "vlib/string-utils.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define FAIL( s )  fail( STR_LEN( "ERROR: " s "\n" ) )

using namespace vlib;


enum
{
	Opt_hex_output = 'x',
};

static command::option options[] =
{
	{ "hex",  Opt_hex_output },
	{ NULL },
};

static bool hex_output = false;

static int fail( const char* msg, unsigned len )
{
	must_write( STDERR_FILENO, msg, len );
	
	return 1;
}

static plus::string stringify( const Value& v )
{
	if ( hex_output  &&  v.type() == Value_number )
	{
		return hex( v.number() );
	}
	
	return make_string( v, Stringified_to_reproduce );
}

static void reproduce( const Value& v )
{
	if ( ! v.type() )
	{
		return;
	}
	
	plus::string s = stringify( v ) + "\n";
	
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
	
	define( proc_getenv );
	define( proc_print  );
	define( proc_time   );
	
	int i = 0;
	
	try
	{
		do
		{
			const char* expr = args[ i ];
			
			const Value tree = parse( expr );
			
			reproduce( eval_tree( tree ) );
		}
		while ( ++i < argn );
	}
	catch ( const std::bad_alloc& )
	{
		return FAIL( "Out of memory!" );
	}
	catch ( const plus::ibox::limb_count_overflow& )
	{
		return FAIL( "Max bigint size exceeded" );
	}
	catch ( const transfer_via_break& )
	{
		return FAIL( "`break` used outside of loop" );
	}
	catch ( const transfer_via_continue& )
	{
		return FAIL( "`continue` used outside of loop" );
	}
	
	return 0;
}
