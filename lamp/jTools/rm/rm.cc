/*	=====
 *	rm.cc
 *	=====
 */

// POSIX
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

// Standard C
#include <stdlib.h>
#include <string.h>

// more-libc
#include "more/string.h"

// more-posix
#include "more/perror.hh"

// command
#include "command/get_option.hh"

// plus
#include "plus/string.hh"

// Orion
#include "Orion/Main.hh"


#define PROGRAM  "rm"
#define PREFIX   PROGRAM ": "

#define STRLEN( s )   (sizeof "" s - 1)
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

static inline
void print_removed( const char* path )
{
	size_t path_len = strlen( path );
	
	size_t size = STRLEN( "removed ''\n" ) + path_len;
	
	char* buffer = (char*) alloca( size );
	
	char* p = buffer;
	
	p = (char*) mempcpy( p, STR_LEN( "removed '" ) );
	p = (char*) mempcpy( p, path, path_len );
	p = (char*) mempcpy( p, STR_LEN( "'\n" ) );
	
	write( STDOUT_FILENO, buffer, size );
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


static inline
bool name_is_dots( const char* name )
{
	return name[0] == '.'  &&  ( name[ 1 + (name[1] == '.') ] == '\0' );
}

static void delete_file( const char* path )
{
	int unlinked = unlink( path );
	
	if ( unlinked < 0  &&  !(globally_forced && errno == ENOENT) )
	{
		more::perror( PROGRAM, path );
		
		global_exit_status = 1;
	}
}

static
plus::string path_descent( const char* path, const char* name )
{
	size_t length = strlen( name );
	
	const size_t path_size = strlen( path );
	
	const bool has_trailing_slash = path[ path_size - 1 ] == '/';
	
	const size_t size = path_size + ! has_trailing_slash + length;
	
	plus::string result;
	
	char* p = result.reset_nothrow( size );
	
	if ( p == NULL )
	{
		_exit( ERROR( 108, "out of memory!" ) );
	}
	
	p = (char*) mempcpy( p, path, path_size );
	
	if ( ! has_trailing_slash )
	{
		*p++ = '/';
	}
	
	p = (char*) mempcpy( p, name, length );
	
	return result.move();
}

static
void recursively_delete_subtrees( const char* path );

static
void recursively_delete( const char* path )
{
	struct stat st;
	
	int nok = lstat( path, &st );
	
	if ( nok < 0  &&  ! (globally_forced  &&  errno == ENOENT) )
	{
		more::perror( PROGRAM, path );
		
		global_exit_status = 1;
		
		return;
	}
	
	if ( ! S_ISDIR( st.st_mode ) )
	{
		delete_file( path );
	}
	else
	{
		recursively_delete_subtrees( path );
		
		nok = rmdir( path );
		
		if ( nok < 0  &&  ! (globally_forced  &&  errno == EACCES) )
		{
			more::perror( PROGRAM, path );
		}
	}
}

static inline
void recursively_delete_tree( const plus::string& path )
{
	recursively_delete( path.data() );  // morally c_str()
}

static
void recursively_delete_subtrees( const char* path )
{
	DIR* dir = opendir( path );
	
	if ( dir == NULL )
	{
		more::perror( PROGRAM, path );
		
		return;
	}
	
	while ( const dirent* entry = readdir( dir ) )
	{
		if ( ! name_is_dots( entry->d_name ) )
		{
			recursively_delete_tree( path_descent( path, entry->d_name ) );
		}
	}
	
	closedir( dir );
}


namespace tool
{
	
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
		
		deleter_f deleter = recursive ? recursively_delete
		                              : delete_file;
		
		for ( int index = 0;  index < argn;  ++index )
		{
			const char* path = args[ index ];
			
			if ( verbose )
			{
				print_removed( path );
			}
			
			deleter( path );
		}
		
		return global_exit_status;
	}
	
}
