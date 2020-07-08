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
#include "vlib/tracker.hh"
#include "vlib/type_info.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/type.hh"

// varyx-mac
#include "varyx/mac/AECreateAppleEvent.hh"
#include "varyx/mac/AEDesc.hh"
#include "varyx/mac/AEFlatten.hh"
#include "varyx/mac/AEPrint.hh"
#include "varyx/mac/AESend.hh"
#include "varyx/mac/Delay.hh"
#include "varyx/mac/PackBits.hh"
#include "varyx/mac/Sound.hh"
#include "varyx/mac/Timer.hh"

// varyx-posix
#include "varyx/posix/argv.hh"
#include "varyx/posix/library.hh"


#define PROGRAM  "macvx"

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define FAIL( s )  fail( STR_LEN( "ERROR: " s "\n" ) )

namespace p7 = poseven;

using namespace command::constants;
using namespace vlib;

using namespace varyx::mac;
using namespace varyx::posix;


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

static const char* inline_script = NULL;

static lexical_scope globals;


static
int fail( const char* msg, unsigned len )
{
	must_write( STDERR_FILENO, msg, len );
	
	return 1;
}

static
char* const* get_options( char** argv )
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
			default:
				break;
		}
	}
	
	return argv;
}

static
Symbol& declare( const char* name )
{
	return *globals.declare( name, Symbol_const ).sym();
}

static
void define( const char* name, const Value& v )
{
	declare( name ).assign( v );
}

static
void define( const proc_info& proc )
{
	if ( proc.addr != NULL )
	{
		define( proc.name, Proc( proc ) );
	}
}

static
void define( const type_info& type )
{
	if ( type.assign != NULL )
	{
		define( type.name, Type( type ) );
	}
}

int main( int argc, char** argv )
{
	if ( argc == 0 )
	{
		return 0;
	}
	
	char* const* args = get_options( argv );
	
	const char* arg0 = inline_script ? "-e"
	                 : args[ 0 ]     ? NULL
	                 :                 "-";
	
	set_argv( declare( "argv" ), arg0, args );
	
	define( proc_exit   );
	define( proc_getenv );
	define( proc_print  );
	define( proc_warn   );
	
	define( AEDesc_vtype  );
	
	define( proc_AECreateAppleEvent );
	define( proc_AEFlatten );
	define( proc_AEPrint );
	define( proc_AESend );
	
	define( proc_Delay );
	define( proc_Microseconds );
	
#if ! __LP64__
	
	define( proc_PackBits );
	define( proc_SysBeep );
	
#endif
	
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
		
		cull_unreachable_objects();
	}
	catch ( const std::bad_alloc& )
	{
		return FAIL( "Out of memory!" );
	}
	catch ( const p7::errno_t& err )
	{
		more::perror( PROGRAM, path, err );
		
		return 1;
	}
	
	return 0;
}
