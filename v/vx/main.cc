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
#include "vlib/interpret.hh"
#include "vlib/scope.hh"
#include "vlib/types.hh"
#include "vlib/types/integer.hh"

// vx
#include "library.hh"
#include "posixfs.hh"

// vx
#include "library.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define FAIL( s )  fail( STR_LEN( "ERROR: " s "\n" ) )

namespace p7 = poseven;

using namespace command::constants;
using namespace vlib;


enum
{
	Opt_unrestricted  = 'Z',
	Opt_inline_script = 'e',
};

static command::option options[] =
{
	{ "inline-script",  Opt_inline_script, Param_required },
	{ "unrestricted",   Opt_unrestricted },
	{ NULL }
};

static bool unrestricted = false;

static const char* inline_script = NULL;

static lexical_scope globals;


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
			
			case Opt_unrestricted:
				unrestricted = true;
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
	
	const Value& argv_symbol = globals.declare( "argv", Symbol_const );
	
	Symbol& sym = *argv_symbol.sym();
	
	sym.denote( Value( c_str_vtype, Op_subscript, empty_list ) );
	
	sym.assign( make_array( argv ) );
}

static
void define( const proc_info& proc )
{
	globals.declare( proc.name, Symbol_const ).sym()->assign( proc );
}

static
void define( const char* name, int i )
{
	globals.declare( name, Symbol_const ).sym()->assign( Integer( i ) );
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
	
	define( "IN",  0 );
	define( "OUT", 1 );
	define( "ERR", 2 );
	
	if ( unrestricted )
	{
		define( proc_SYSTEM );
	}
	else
	{
		define( proc_system );
	}
	
	define( proc_close   );
	define( proc_dirname );
	define( proc_dup     );
	define( proc_dup2    );
	define( proc_eval    );
	define( proc_fstat   );
	define( proc_getenv  );
	define( proc_listdir );
	define( proc_load    );
	define( proc_lstat   );
	define( proc_pipe    );
	define( proc_print   );
	define( proc_read    );
	define( proc_sleep   );
	define( proc_stat    );
	define( proc_time    );
	define( proc_write   );
	
	const char* path = "<inline script>";
	
	try
	{
		if ( inline_script != NULL )
		{
			interpret( inline_script, NULL, &globals );
		}
		else
		{
			path = args[ 0 ] ? args++[ 0 ] : "/dev/fd/0";
			
			plus::string program = p7::slurp( path );
			
			if ( strlen( program.c_str() ) != program.size() )
			{
				return FAIL( "Program contains NUL bytes" );
			}
			
			interpret( program.c_str(), path, &globals );
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
