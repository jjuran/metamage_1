/*
	main.cc
	-------
*/

// POSIX
#include <unistd.h>

// Standard C
#include <string.h>

// Standard C++
#include <new>

// must
#include "must/write.h"

// more-posix
#include "more/perror.hh"

// command
#include "command/get_option.hh"

// poseven
#include "poseven/extras/slurp.hh"
#include "poseven/types/errno_t.hh"

// vlib
#include "vlib/calc.hh"
#include "vlib/init.hh"
#include "vlib/library.hh"
#include "vlib/parse.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define FAIL( s )  fail( STR_LEN( "ERROR: " s "\n" ) )

namespace p7 = poseven;

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
		else
		{
			path = args[ 0 ] ? args++[ 0 ] : "/dev/fd/0";
			
			plus::string program = p7::slurp( path );
			
			if ( strlen( program.c_str() ) != program.size() )
			{
				FAIL( "Program contains NUL bytes" );
			}
			
			execute( program.c_str() );
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
	catch ( const p7::errno_t& err )
	{
		more::perror( "vx", path, err );
		
		return 1;
	}
	
	return 0;
}
