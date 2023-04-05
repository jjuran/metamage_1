/*
	select.cc
	---------
*/

// Standard C
#include <errno.h>
#include <stdlib.h>
#include <string.h>

// POSIX
#include <fcntl.h>
#include <sys/select.h>
#include <sys/uio.h>
#include <unistd.h>

// more-posix
#include "more/perror.hh"

// command
#include "command/get_option.hh"

// vxo
#include "vxo/ptrvec.hh"


#pragma exceptions off


#define PROGRAM  "select"

#define STR_LEN( s )  "" s, (sizeof s - 1)


using namespace command::constants;

enum
{
	Option_only_one = '1',
	Option_reader   = 'r',
};

static command::option options[] =
{
	{ "", Option_only_one, Param_unwanted },
	{ "", Option_reader,   Param_required },
	
	{ NULL }
};

static bool only_one = false;

static vxo::UPtrVec< const char > readers;

static inline
void report_error( const char* path, int err = errno )
{
	more::perror( PROGRAM, path, err );
}

static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_only_one:
				only_one = true;
				break;
			
			case Option_reader:
				if ( readers.push_back_nothrow( command::global_result.param ) )
				{
					break;  // ok
				}
				
				// Out of memory already!?
				// Fall through
			
			default:
				abort();
		}
	}
	
	return argv;
}


static int Select( const vxo::UPtrVec< const char >& read_files, bool only_one )
{
	int max_fd_plus_1 = 0;
	
	fd_set read_fds;
	
	FD_ZERO( &read_fds );
	
	vxo::UPtrVec< const char > name_of;
	
	typedef vxo::UPtrVec< const char >::const_iterator const_iterator;
	
	for ( const_iterator it = read_files.begin();  it != read_files.end();  ++it )
	{
		const char* name = *it;
		
		int fd = open( name, O_RDONLY );
		
		if ( fd < 0 )
		{
			report_error( name );
			return 5;
		}
		
		FD_SET( fd, &read_fds );
		
		max_fd_plus_1 = fd + 1;
		
		if ( ! name_of.resize_nothrow( max_fd_plus_1 ) )
		{
			// The only conceivable error is ENOMEM.
			report_error( "<memory allocation>" );
			return 108;
		}
		
		name_of[ fd ] = name;
	}
	
	// This blocks and yields to other threads
	int selected = select( max_fd_plus_1, &read_fds, NULL, NULL, NULL );
	
	if ( selected < 0 )
	{
		report_error( "<select>" );
		
		return 3;  // error from select()
	}
	
	if ( selected == 0 )
	{
		return 1;  // timeout
	}
	
	if ( selected > 1  &&  only_one )
	{
		return 2;  // multiple, maybe a window closed
	}
	
	for ( int reader = 0;  reader < max_fd_plus_1;  ++reader )
	{
		if ( !FD_ISSET( reader, &read_fds ) )  continue;
		
		const char* name = name_of[ reader ];
		
		iovec iov[] =
		{
			{ (char*) name, strlen( name ) },
			{ (char*)      STR_LEN( "\n" ) },
		};
		
		ssize_t n = writev( STDOUT_FILENO, iov, sizeof iov / sizeof *iov );
		
		if ( n < 0 )
		{
			return 4;
		}
	}
	
	return 0;
}


int main( int argc, char** argv )
{
	char *const *args = get_options( argv );
	
	return Select( readers, only_one );
}
