/*
	statusfsd.cc
	------------
*/

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <stdlib.h>

// command
#include "command/get_option.hh"

// vfs
#include "vfs/node.hh"

// statusfs
#include "statusfs.hh"

// freemount
#include "freemount/event_loop.hh"
#include "freemount/receiver.hh"

// freemountd
#include "freemount/server.hh"
#include "freemount/session.hh"


using namespace command::constants;
using namespace freemount;


enum
{
	Option_quiet = 'q',
	Option_user  = 'u',
};

static command::option options[] =
{
	{ "quiet", Option_quiet },
	{ "user",  Option_user },
	{ NULL }
};

static uid_t the_user = -1;


static const vfs::node& root()
{
	static vfs::node_ptr root = vfs::fixed_dir( the_user, statusfs::mappings );
	
	return *root;
}


static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_quiet:
				int dev_null;
				dev_null = open( "/dev/null", O_WRONLY );
				
				dup2( dev_null, STDERR_FILENO );
				
				close( dev_null );
				break;
			
			case Option_user:
				the_user = 0;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}

int main( int argc, char* const* argv )
{
	char *const *args = get_options( argv );
	
	session s( STDOUT_FILENO, root(), root() );
	
	data_receiver r( &frame_handler, &s );
	
	int looped = run_event_loop( r, STDIN_FILENO );
	
	return looped != 0;
}
