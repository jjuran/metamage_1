/*	=====
 *	rm.cc
 *	=====
 */

// POSIX
#include <unistd.h>

// Standard C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// command
#include "command/get_option.hh"

// pfiles
#include "pfiles/common.hh"

// Io
#include "io/walk.hh"

// Orion
#include "Orion/Main.hh"


#define PREFIX  "rm: "

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define ERROR( e, msg )  (wrote( 2, STR_LEN( PREFIX msg "\n" ) ) ? e : 13)


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

static inline
bool wrote( int fd, const void* buffer, size_t n )
{
	return write( fd, buffer, n ) == n;
}

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


static int global_exit_status = 0;


namespace tool
{
	
	static void delete_file( const char* path )
	{
		int unlinked = unlink( path );
		
		if ( unlinked < 0  &&  !(globally_forced && errno == ENOENT) )
		{
			fprintf( stderr, "rm: %s: %s\n", path, strerror( errno ) );
			
			global_exit_status = 1;
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
			return ERROR( 50, "missing arguments" );
		}
		
		typedef void (*deleter_f)(const char*);
		
		deleter_f deleter = recursive ? recursive_delete
		                              : delete_file;
		
		for ( int index = 0;  index < argn;  ++index )
		{
			const char* path = args[ index ];
			
			if ( verbose )
			{
				printf( "removed '%s'\n", path );
			}
			
			deleter( path );
		}
		
		return global_exit_status;
	}
	
}
