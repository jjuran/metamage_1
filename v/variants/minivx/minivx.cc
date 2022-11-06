/*
	minivx.cc
	---------
*/

// POSIX
#include <unistd.h>

// Standard C
#include <signal.h>
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
#include "vlib/types/integer.hh"
#include "vlib/types/proc.hh"
#include "vlib/types/string.hh"
#include "vlib/types/type.hh"
#include "vlib/types/unitary.hh"

// varyx-meta
#include "varyx/meta/eval.hh"

// varyx-posix
#include "varyx/posix/argv.hh"
#include "varyx/posix/file_descriptor.hh"
#include "varyx/posix/library.hh"
#include "varyx/posix/posixfs.hh"
#include "varyx/posix/signals.hh"


#define PROGRAM  "minivx"

#define STRLEN( s )  (sizeof "" s - 1)

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define FAIL( s )  fail( STR_LEN( "ERROR: " s "\n" ) )

#define END  "\n" "end." "\n"

namespace p7 = poseven;

using namespace command::constants;
using namespace vlib;

using namespace varyx::meta;
using namespace varyx::posix;


enum
{
	Opt_unrestricted  = 'Z',
	Opt_check_syntax  = 'c',
	Opt_inline_script = 'e',
};

static command::option options[] =
{
	{ "inline-script",  Opt_inline_script, Param_required },
	{ "unrestricted",   Opt_unrestricted },
	{ "",               Opt_check_syntax },
	{ NULL }
};

static bool check_syntax = false;
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
			
			case Opt_check_syntax:
				check_syntax = true;
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
	define( proc.name, Proc( proc ) );
}

static
void define( const type_info& type )
{
	define( type.name, Type( type ) );
}

static
void define( const char* name, int i )
{
	define( name, FileDescriptor( i ) );
}

static
void define( const char* name )
{
	define( name, Unitary( name ) );
}

int main( int argc, char** argv )
{
	if ( argc == 0 )
	{
		return 0;
	}
	
	char* const* args = get_options( argv );
	
	(void) signal( SIGPIPE, SIG_IGN );
	
	const int argn = argc - (args - argv);
	
	const char* arg0 = inline_script ? "-e"
	                 : args[ 0 ]     ? NULL
	                 :                 "-";
	
	set_argv( declare( "argv" ), arg0, args );
	
	define( "argc", Integer( argn + (arg0 != NULL) ) );
	
	define( "IN",  0 );
	define( "OUT", 1 );
	define( "ERR", 2 );
	
	define( "FIN" );
	
	if ( unrestricted )
	{
		define( proc_EXECV  );
		define( proc_EXECVP );
		define( proc_RUN    );
		define( proc_RUNOUT );
		define( proc_SYSTEM );
	}
	else
	{
		define( proc_system );
	}
	
	if ( unrestricted )
	{
		define( proc_append );
		define( proc_chdir  );
		define( proc_link   );
		define( proc_mkdir  );
		define( proc_remove );
		define( proc_rename );
		define( proc_rewrite );
		define( proc_touch  );
		define( proc_truncate );
	}
	
	define( fd_vtype      );
	
	define( proc_close    );
	define( proc_dirname  );
	define( proc_dup      );
	define( proc_dup2     );
	define( proc_eval     );
	define( proc_exit     );
	define( proc_fstat    );
	define( proc_getenv   );
	define( proc_getpid   );
	define( proc_isatty   );
	define( proc_listdir  );
	define( proc_load     );
	define( proc_lstat    );
	define( proc_pipe     );
	define( proc_print    );
	define( proc_read     );
	define( proc_reader   );
	define( proc_readlink );
	define( proc_realpath );
	define( proc_setenv   );
	define( proc_sig_dfl  );
	define( proc_sig_ign  );
	define( proc_sleep    );
	define( proc_stat     );
	define( proc_symlink  );
	define( proc_sync     );
	define( proc_time     );
	define( proc_utime    );
	define( proc_warn     );
	define( proc_write    );
	
	const char* path = "<inline script>";
	
	execute_proc execute = check_syntax ? identity_function : NULL;
	
	try
	{
		if ( inline_script != NULL )
		{
			interpret( inline_script, NULL, &globals, NULL, execute );
		}
		else
		{
			path = args[ 0 ] ? args++[ 0 ] : "/dev/fd/0";
			
			plus::string program = p7::slurp( path );
			
			if ( strlen( program.c_str() ) != program.size() )
			{
				return FAIL( "Program contains NUL bytes" );
			}
			
			plus::string::size_type pos = program.find( END );
			
			if ( pos != plus::string::npos )
			{
				const plus::string::size_type data_pos = pos + STRLEN( END );
				
				define( "DATA", String( program.substr( data_pos ) ) );
				
				++pos;
			}
			
			define( "CODE", String( program.substr( 0, pos ) ) );
			
			interpret( program.c_str(), path, &globals, NULL, execute );
			
			if ( check_syntax )
			{
				write( STDERR_FILENO, path, strlen( path ) );
				write( STDERR_FILENO, STR_LEN( " syntax OK\n" ) );
			}
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
