/*
	json-reader.cc
	--------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// Standard C
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// more-posix
#include "more/perror.hh"

// command
#include "command/get_option.hh"

// vxo
#include "vxo/container.hh"
#include "vxo/error.hh"
#include "vxo/ptrvec.hh"

// vxs
#include "vxs/lib/json/decode.hh"
#include "vxs/string.hh"


#pragma exceptions off


#define PROGRAM  "json-count"


using namespace command::constants;

using vxo::Box;
using vxo::Container;
using vxo::Error;
using vxo::String;


enum
{
	Option_file = 'f',
	Option_key  = 'k',
};

static command::option options[] =
{
	{ "", Option_file, Param_required },
	{ "", Option_key,  Param_required },
	
	{ NULL }
};

static vxo::UPtrVec< const char > keys;
static vxo::UPtrVec< const char > files;

typedef vxo::UPtrVec< const char >::const_iterator iterator;

static iterator keys_begin;
static iterator keys_end;

static inline
void report_error( const char* path, int err = errno )
{
	more::perror( PROGRAM, path, err );
}

static
char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_file:
				if ( ! files.push_back_nothrow( command::global_result.param ) )
				{
					report_error( "<processing arguments>" );
					exit( 1 );
				}
				break;
			
			case Option_key:
				if ( ! keys.push_back_nothrow( command::global_result.param ) )
				{
					report_error( "<processing arguments>" );
					exit( 1 );
				}
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}


static inline
int count( const Box& box )
{
	if ( const Container* container = box.is< Container >() )
	{
		return container->size();
	}
	
	return 1;
}

static
int count( const Box& box, const char* key )
{
	if ( const Container* container = box.is< Container >() )
	{
		const Container& box = *container;
		
		const size_t n = box.size();
		
		int total = 0;
		
		if ( const int is_map = box.subtype_byte() & 1 )
		{
			for ( size_t i = 0;  i < n;  ++i )
			{
				if ( static_cast< const String& >( box[ i++ ] ).get() == key )
				{
					total += count( box[ i ] );
				}
				else
				{
					total += count( box[ i ], key );
				}
			}
		}
		else
		{
			for ( size_t i = 0;  i < n;  ++i )
			{
				total += count( box[ i ], key );
			}
		}
		
		return total;
	}
	
	return 0;
}

static
int count_keys_in_file( int fd, const char* path )
{
	struct stat st;
	
	size_t n;
	
	char* p = NULL;
	
	int nok = fstat( fd, &st )                 ||
	          (! S_ISREG( st.st_mode )  &&
	           (errno = ESPIPE))               ||
	          ! (n = st.st_size,
	             p = (char*) malloc( n + 1 ))  ||
	          read( fd, p, n ) != n;
	
	if ( nok )
	{
		perror( path );
		
		free( p );
		return 1;
	}
	
	p[ n ] = '\0';
	
	if ( strlen( p ) != n )
	{
		fprintf( stderr, "%s: %s\n", PROGRAM, "forbidden NUL byte" );
		
		free( p );
		return 1;
	}
	
	const Box box = vxo::json_decode( p );
	
	if ( const Error* error = box.is< Error >() )
	{
		fprintf( stderr, "%s: %s\n", PROGRAM, error->get() );
		
		free( p );
		return 1;
	}
	
	for ( iterator it = keys_begin;  it != keys_end;  ++it )
	{
		const char* key = *it;
		
		int n = count( box, key );
		
		printf( "%s: %d\n", key, n );
	}
	
	free( p );
	
	return 0;
}

static
int count_keys_in_file( const char* path )
{
	int fd = open( path, O_RDONLY );
	
	if ( fd < 0 )
	{
		perror( path );
		return 1;
	}
	
	int err = count_keys_in_file( fd, path );
	
	close( fd );
	
	return err;
}

/*
	Args/keys/files truth table:
	
	AKF
	000     ERROR: no keys or files
	001     ERROR: no keys
	010     OK: read from stdin
	011     OK:
	100     OK: args are keys, read from stdin
	101     OK: args are keys
	110     OK: args are files
	111     ERROR: args are superfluous
*/

int main( int argc, char** argv )
{
	char *const *args = get_options( argv );
	
	int argn = argc - (args - argv);
	
	size_t n_files = files.size();
	size_t n_keys  = keys.size();
	
	if ( ! argn  &&  ! n_keys  &&  ! n_files )
	{
		fprintf( stderr, "%s: %s\n", PROGRAM, "No files or keys were given" );
		return 2;
	}
	
	if ( ! argn  &&  ! n_keys  &&  n_files )
	{
		fprintf( stderr, "%s: %s\n", PROGRAM, "No keys were given" );
		return 2;
	}
	
	if ( argn  &&  n_keys  &&  n_files )
	{
		fprintf( stderr, "%s: %s\n", PROGRAM, "Files, keys, args:  pick two" );
		return 2;
	}
	
	iterator files_begin = files.cbegin();
	
	keys_begin = keys.cbegin();
	keys_end   = keys_begin + n_keys;
	
	if ( argn  &&  ! n_keys )
	{
		keys_begin = args;
		keys_end   = args + argn;
	}
	else if ( argn  &&  ! n_files )
	{
		files_begin = args;
		n_files     = argn;
	}
	
	if ( ! n_files )
	{
		return count_keys_in_file( STDIN_FILENO, "<stdin>" );
	}
	
	int exit_status = 0;
	
	for ( int i = 0;  i < n_files;  ++i )
	{
		const char* path = files_begin[ i ];
		
		exit_status |= count_keys_in_file( path );
	}
	
	return exit_status;
}
