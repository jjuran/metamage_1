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

// must
#include "must/write.h"

// command
#include "command/get_option.hh"

// plus
#include "plus/decimal.hh"
#include "plus/integer_hex.hh"
#include "plus/string/concat.hh"

// vc
#include "vc/calc.hh"
#include "vc/error.hh"
#include "vc/library.hh"
#include "vc/parse.hh"
#include "vc/quote.hh"
#include "vc/symbol_table.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define FAIL( s )  fail( STR_LEN( "ERROR: " s "\n" ) )

#define ADDRESS_ERROR( s )  VC_ERROR( "ADDRESS", s )


enum
{
	Opt_hex_output = 'x',
};

static command::option options[] =
{
	{ "hex",  Opt_hex_output },
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

static plus::string stringify( const vc::Value& v )
{
	switch ( v.type )
	{
		using namespace vc;
		
		default:
			return "UNIMPLEMENTED";
		
		case Value_nothing:
			return "NOTHING";
		
		case Value_dummy_operand:
			return "DUMMY OPERAND";
		
		case Value_undefined:
			return "UNDEFINED";
		
		case Value_empty_list:
			return "()";
		
		case Value_boolean:
			return v.number.is_zero() ? "false" : "true";
		
		case Value_number:
			return stringify( v.number );
		
		case Value_string:
			return quote_string( v.string );
		
		case Value_function:
			return v.string;
		
		case Value_pair:
			{
				const Expr& expr = *v.expr.get();
				
				return stringify( expr.left ) + ", " + stringify( expr.right );
			}
		
			break;
	}
}

static void reproduce( const vc::Value& v )
{
	if ( ! v.type )
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
int get_int( const vc::Value& v )
{
	if ( v.type != vc::Value_number )
	{
		ADDRESS_ERROR( "PC is non-numeric" );
	}
	
	if ( v.number.is_negative() )
	{
		ADDRESS_ERROR( "PC is negative" );
	}
	
	if ( v.number > 10000 )
	{
		ADDRESS_ERROR( "PC is exorbitant" );
	}
	
	return v.number.clipped();
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
	
	using namespace vc;
	
	define( "getenv", &v_getenv );
	define( "join",   &v_join   );
	define( "print",  &v_print  );
	define( "time",   &v_time   );
	
	symbol_id pc = create_symbol( "PC", Symbol_var );
	
	int i = 0;
	
	do
	{
		const char* expr = args[ i ];
		
		assign_symbol( pc, i + 1 );
		
		try
		{
			reproduce( parse_and_eval( expr ) );
		}
		catch ( const std::bad_alloc& )
		{
			return FAIL( "Out of memory!" );
		}
		catch ( const plus::ibox::limb_count_overflow& )
		{
			return FAIL( "Max bigint size exceeded" );
		}
		
		breathe();
		
		i = get_int( lookup_symbol( pc ) );
	}
	while ( i < argn );
	
	return 0;
}
