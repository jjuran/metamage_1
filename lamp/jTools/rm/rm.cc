/*	=====
 *	rm.cc
 *	=====
 */

// Standard C
#include <stdlib.h>

// Standard C/C++
#include <cstdio>
#include <cstring>

// Iota
#include "iota/strings.hh"

// command
#include "command/get_option.hh"

// poseven
#include "poseven/functions/stat.hh"
#include "poseven/functions/write.hh"
#include "poseven/sequences/directory_contents.hh"
#include "poseven/types/exit_t.hh"

// pfiles
#include "pfiles/common.hh"

// Io
#include "io/walk.hh"

// Orion
#include "Orion/Main.hh"


using namespace command::constants;

enum
{
	Option_Recursive = 'R',
	Option_force     = 'f',
	Option_recursive = 'r',
	Option_verbose   = 'v',
};

static command::option options[] =
{
	{ "", Option_force     },
	{ "", Option_recursive },
	{ "", Option_Recursive },
	{ "", Option_verbose   },
	
	{ NULL }
};

static bool globally_forced = false;
static bool recursive = false;
static bool verbose = false;

static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_force:
				globally_forced = true;
				break;
			
			case Option_recursive:
			case Option_Recursive:
				recursive = true;
				break;
			
			case Option_verbose:
				verbose = true;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}


namespace tool
{
	
	namespace p7 = poseven;
	
	
	static p7::exit_t global_exit_status = p7::exit_success;
	
	
	static void delete_file( const char* path )
	{
		int unlinked = ::unlink( path );
		
		if ( unlinked < 0  &&  !(globally_forced && errno == ENOENT) )
		{
			std::fprintf( stderr, "rm: %s: %s\n", path, std::strerror( errno ) );
			
			global_exit_status = p7::exit_failure;
		}
	}
	
	
	struct file_deleter
	{
		void operator()( const plus::string& path, unsigned depth ) const
		{
			delete_file( path.c_str() );
		}
	};
	
	
	static void recursive_delete( const char* path )
	{
		if ( globally_forced && !io::item_exists( path ) )
		{
			return;
		}
		
		recursively_walk_tree( plus::string( path ),
		                       io::walk_noop(),
		                       file_deleter(),
		                       io::directory_deleter< plus::string >() );
	}
	
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		// Check for sufficient number of args
		if ( argn < 1 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "rm: missing arguments\n" ) );
			
			return p7::exit_failure;
		}
		
		typedef void (*deleter_f)(const char*);
		
		deleter_f deleter = recursive ? recursive_delete
		                              : delete_file;
		
		for ( int index = 0;  index < argn;  ++index )
		{
			const char* path = args[ index ];
			
			if ( verbose )
			{
				std::printf( "removed '%s'\n", path );
			}
			
			deleter( path );
		}
		
		return global_exit_status;
	}
	
}
