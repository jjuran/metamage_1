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
#include "vlib/init.hh"
#include "vlib/interpret.hh"
#include "vlib/library.hh"
#include "vlib/symbol_table.hh"


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

static
Value make_argv( int argn, char* const* args )
{
	char* const* argp = args + argn;
	
	Value result = *--argp;
	
	while ( argp > args )
	{
		result = Value( *--argp, result );
	}
	
	return result;
}

static
void set_argv( const char* arg0, int argn, char* const* args )
{
	Value argv = Value_empty_list;
	
	if ( argn )
	{
		argv = make_argv( argn, args );
		
		if ( arg0 )
		{
			argv = Value( arg0, argv );
		}
	}
	else if ( arg0 )
	{
		argv = arg0;
	}
	
	symbol_id argv_sym = create_symbol( "argv", Symbol_const );
	
	assign_symbol( argv_sym, make_array( argv ) );
}

int main( int argc, char** argv )
{
	if ( argc == 0 )
	{
		return 0;
	}
	
	char* const* args = get_options( argv );
	
	const int argn = argc - (args - argv);
	
	const char* arg0 = inline_script ? "-e"
	                 : args[ 0 ]     ? NULL
	                 :                 "-";
	
	set_argv( arg0, argn, args );
	
	define( proc_getenv );
	define( proc_print  );
	define( proc_time   );
	
	const char* path = "<inline script>";
	
	try
	{
		if ( inline_script != NULL )
		{
			interpret( inline_script );
		}
		else
		{
			path = args[ 0 ] ? args++[ 0 ] : "/dev/fd/0";
			
			plus::string program = p7::slurp( path );
			
			if ( strlen( program.c_str() ) != program.size() )
			{
				return FAIL( "Program contains NUL bytes" );
			}
			
			interpret( program.c_str(), path );
		}
	}
	catch ( const std::bad_alloc& )
	{
		return FAIL( "Out of memory!" );
	}
	catch ( const p7::errno_t& err )
	{
		more::perror( "vx", path, err );
		
		return 1;
	}
	
	return 0;
}
