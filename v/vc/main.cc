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
#include "plus/decimal.hh"
#include "plus/integer_hex.hh"
#include "plus/string/concat.hh"

// vc
#include "vc/parse.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define FAIL( s )  fail( STR_LEN( "ERROR: " s "\n" ) )


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
		
		case Value_boolean:
			return v.number.is_zero() ? "false" : "true";
		
		case Value_number:
			return stringify( v.number );
	}
}

static void print( const vc::Value& v )
{
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
	
	while ( const char* expr = *args++ )
	{
		try
		{
			print( vc::parse_and_eval( expr ) );
		}
		catch ( const std::bad_alloc& )
		{
			return FAIL( "Out of memory!" );
		}
		catch ( const plus::ibox::limb_count_overflow& )
		{
			return FAIL( "Max bigint size exceeded" );
		}
	}
	
	return 0;
}
