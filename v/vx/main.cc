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

// vlib
#include "vlib/calc.hh"
#include "vlib/init.hh"
#include "vlib/library.hh"
#include "vlib/parse.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define FAIL( s )  fail( STR_LEN( "ERROR: " s "\n" ) )

using namespace command::constants;
using namespace vlib;


enum
{
	Opt_inline_script = 'e',
};

static command::option options[] =
{
	{ "inline-script",  Opt_inline_script, Param_required },
	{ NULL }
};

static const char* inline_script = NULL;

static int fail( const char* msg, unsigned len )
{
	must_write( STDERR_FILENO, msg, len );
	
	return 1;
}


static
void execute( const char* program )
{
	eval_tree( parse( program ) );
}

static char* const* get_options( char** argv )
{
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		switch ( opt )
		{
			case Opt_inline_script:
				inline_script = command::global_result.param;
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
	
	define( proc_getenv );
	define( proc_print  );
	define( proc_time   );
	
	const char* path = "<inline script>";
	
	try
	{
		if ( inline_script != NULL )
		{
			execute( inline_script );
		}
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
