/*
	main.cc
	-------
*/

// POSIX
#include <unistd.h>

// Standard C
#include <signal.h>

// Standard C++
#include <new>
#include <vector>

// must
#include "must/write.h"

// command
#include "command/get_option.hh"

// plus
#include "plus/decimal.hh"
#include "plus/integer_hex.hh"
#include "plus/string/concat.hh"

// vlib
#include "vlib/calc.hh"
#include "vlib/error.hh"
#include "vlib/functions.hh"
#include "vlib/init.hh"
#include "vlib/library.hh"
#include "vlib/parse.hh"
#include "vlib/string-utils.hh"
#include "vlib/symbol_table.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define FAIL( s )  fail( STR_LEN( "ERROR: " s "\n" ) )

#define ADDRESS_ERROR( s )  VLIB_ERROR( "ADDRESS", s )

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

static plus::string stringify( const plus::integer& i )
{
	return hex_output ? hex( i ) : encode_decimal( i );
}

static plus::string stringify( const Value& v )
{
	if ( v.type() == Value_number )
	{
		return stringify( vlib::get_int( v ) );
	}
	
	return make_string( v, Stringified_to_reproduce );
}

static void reproduce( const Value& v )
{
	if ( ! get_type( v ) )
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

static inline
void breathe()
{
#ifdef __RELIX__
	
	kill( 1, 0 );
	
#endif
}

static
int get_int( const Value& v )
{
	if ( get_type( v ) != Value_number )
	{
		ADDRESS_ERROR( "PC is non-numeric" );
	}
	
	if ( vlib::get_int( v ).is_negative() )
	{
		ADDRESS_ERROR( "PC is negative" );
	}
	
	if ( vlib::get_int( v ) > 10000 )
	{
		ADDRESS_ERROR( "PC is exorbitant" );
	}
	
	return vlib::get_int( v ).clipped();
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
	
	std::vector< Value > parse_trees( argn );
	
	define( proc_getenv );
	define( proc_print  );
	define( proc_time   );
	
	symbol_id pc = create_symbol( "PC", Symbol_var );
	
	int i = 0;
	
	try
	{
		do
		{
			const char* expr = args[ i ];
			
			assign_symbol( pc, i + 1 );
			
			if ( parse_trees[ i ].type() == Value_nothing )
			{
				parse_trees[ i ] = parse( expr );
			}
			
			const Value& tree = parse_trees[ i ];
			
			reproduce( eval_tree( tree ) );
			
			breathe();
			
			i = ::get_int( lookup_symbol( pc ) );
		}
		while ( i < argn );
	}
	catch ( const std::bad_alloc& )
	{
		return FAIL( "Out of memory!" );
	}
	catch ( const plus::ibox::limb_count_overflow& )
	{
		return FAIL( "Max bigint size exceeded" );
	}
	
	return 0;
}
