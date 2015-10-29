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
#include "plus/var_string.hh"

// vlib
#include "vlib/calc.hh"
#include "vlib/error.hh"
#include "vlib/functions.hh"
#include "vlib/library.hh"
#include "vlib/parse.hh"
#include "vlib/quote.hh"
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

static plus::string stringify_list( const Value& v );

static plus::string stringify( const Value& v )
{
	switch ( v.type )
	{
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
			return stringify_list( v );
	}
}

static plus::string stringify_list( const Value& v )
{
	plus::var_string result = "(";
	
	const Value* next = &v;
	
	if ( v.expr.get()->op != Op_list )
	{
		const char* op_token = " % ";
		
		Expr& expr = *v.expr.get();
		
		result += stringify( expr.left );
		result += op_token;
		result += stringify( expr.right );
		result += ")";
		
		return result.move();
	}
	
	while ( const Expr* expr = next->expr.get() )
	{
		if ( expr->op != Op_list )
		{
			break;
		}
		
		next = &expr->right;
		
		result += stringify( expr->left );
		
		result += ", ";
	}
	
	result += stringify( *next );
	result += ")";
	
	return result.move();
}

static void reproduce( const Value& v )
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
int get_int( const Value& v )
{
	if ( v.type != Value_number )
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
	
	define( "getenv", &v_getenv );
	define( "join",   &v_join   );
	define( "print",  &v_print  );
	define( "rep",    &v_rep    );
	define( "time",   &v_time   );
	define( "unbin",  &v_unbin  );
	define( "unhex",  &v_unhex  );
	
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
