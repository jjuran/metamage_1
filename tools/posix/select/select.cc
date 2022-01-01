/*
	select.cc
	---------
*/

// Standard C
#include <stdlib.h>

// Standard C++
#include <algorithm>
#include <map>
#include <vector>

// POSIX
#include <sys/select.h>

// Iota
#include "iota/strings.hh"

// command
#include "command/get_option.hh"

// plus
#include "plus/string/concat.hh"

// poseven
#include "poseven/functions/open.hh"
#include "poseven/functions/perror.hh"
#include "poseven/functions/write.hh"
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
		int maxFD = -1;
		
		fd_set read_fds;
		
		FD_ZERO( &read_fds );
		
		std::map< int, const char* > name_of;
		
		typedef std::vector< const char* >::const_iterator const_iterator;
		
		for ( const_iterator it = read_files.begin();  it != read_files.end();  ++it )
		{
			const char* name = *it;
			
			p7::fd_t fd = p7::open( name, p7::o_rdonly ).release();
			
			FD_SET( fd, &read_fds );
			
			maxFD = std::max< int >( maxFD, fd );
			
			name_of[ fd ] = name;
		}
		
		// This blocks and yields to other threads
		int selected = select( maxFD + 1, &read_fds, NULL, NULL, NULL );
		
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
		
		for ( int reader = 0;  reader <= maxFD;  ++reader )
		{
			if ( !FD_ISSET( reader, &read_fds ) )  continue;
			
			const char* name = name_of[ reader ];
			
			const plus::string message = plus::concat( name, STR_LEN( "\n" ) );
			
			p7::write( p7::stdout_fileno, message );
		}
		
		return 0;
	}
	
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		return Select( readers, only_one );
	}
	
}
