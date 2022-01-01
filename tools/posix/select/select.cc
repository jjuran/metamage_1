/*
	select.cc
	---------
*/

// Standard C
#include <stdlib.h>
#include <string.h>

// Standard C++
#include <vector>

// POSIX
#include <fcntl.h>
#include <sys/select.h>
#include <sys/uio.h>
#include <unistd.h>

// Iota
#include "iota/strings.hh"

// command
#include "command/get_option.hh"

// poseven
#include "poseven/functions/perror.hh"
#include "poseven/types/exit_t.hh"

// Orion
#include "Orion/Main.hh"


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

static std::vector< const char* > readers;

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
				readers.push_back( command::global_result.param );
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
	
	
	static int Select( const std::vector< const char* >& read_files, bool only_one )
	{
		int max_fd_plus_1 = 0;
		
		fd_set read_fds;
		
		FD_ZERO( &read_fds );
		
		std::vector< const char* > name_of;
		
		typedef std::vector< const char* >::const_iterator const_iterator;
		
		for ( const_iterator it = read_files.begin();  it != read_files.end();  ++it )
		{
			const char* name = *it;
			
			int fd = open( name, O_RDONLY );
			
			if ( fd < 0 )
			{
				p7::perror( "select", name );
				return 5;
			}
			
			FD_SET( fd, &read_fds );
			
			max_fd_plus_1 = fd + 1;
			
			name_of.resize( max_fd_plus_1 );
			
			name_of[ fd ] = name;
		}
		
		// This blocks and yields to other threads
		int selected = select( max_fd_plus_1, &read_fds, NULL, NULL, NULL );
		
		if ( selected < 0 )
		{
			p7::perror( "select: select()" );
			
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
	
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		return Select( readers, only_one );
	}
	
}
