/*
	buffer.cc
	---------
*/

// Standard C
#include <stdlib.h>
#include <string.h>

// POSIX
#include <sys/ttycom.h>

// Iota
#include "iota/strings.hh"

// Relix
#include "relix/fork_and_exit.h"

// command
#include "command/get_option.hh"

// poseven
#include "poseven/extras/pump.hh"
#include "poseven/extras/splat.hh"
#include "poseven/functions/chdir.hh"
#include "poseven/functions/ftruncate.hh"
#include "poseven/functions/ioctl.hh"
#include "poseven/functions/link.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/openat.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/symlink.hh"
#include "poseven/functions/utime.hh"
#include "poseven/functions/vfork.hh"
#include "poseven/functions/wait.hh"
#include "poseven/functions/write.hh"
#include "poseven/functions/_exit.hh"

// Orion
#include "Orion/Main.hh"


using namespace command::constants;

enum
{
	Option_output = 'o',
	Option_title  = 't',
	Option_wait   = 'w',
	
	Option_last_byte = 255,
	
	Option_in_place,
};

static command::option options[] =
{
	{ "out",   Option_output, Param_required },
	{ "title", Option_title,  Param_required },
	
	{ "wait",     Option_wait      },
	{ "in-place", Option_in_place  },
	
	{ NULL }
};

static bool should_wait = false;
static bool in_place    = false;

static const char* title       = NULL;
static const char* output_file = "/dev/null";

static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_output:
				output_file = command::global_result.param;
				break;
			
			case Option_title:
				title = command::global_result.param;
				break;
			
			case Option_wait:
				should_wait = true;
				break;
			
			case Option_in_place:
				in_place = true;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	static void make_window( const char* title )
	{
		p7::chdir( "/gui/new/port" );
		
		p7::splat( p7::open( ".~title", p7::o_wronly | p7::o_trunc ),
		           title,
		           strlen( title ) );
		
		const short width  = 2 * 4 +  6 * 80 + 15;
		const short height = 2 * 4 + 11 * 24;
		
		p7::splat( "size", STR_LEN( "503x272" "\n" ) );
		
		p7::utime( "window" );
		
		p7::splat( "w/text-font", STR_LEN( "4" "\n" ) );
		p7::splat( "w/text-size", STR_LEN( "9" "\n" ) );
		
		p7::link( "/gui/new/stack",       "view"          );
		
		p7::link( "/gui/new/scrollframe", "v/main/view"     );
		p7::link( "/gui/new/frame",       "v/main/v/view"   );
		p7::link( "/gui/new/textedit",    "v/main/v/v/view" );
		
		p7::symlink( "v/v", "v/main/v/target" );
		
		p7::splat( "v/main/v/vertical",  STR_LEN( "1" "\n" ) );
		p7::splat( "v/main/v/v/padding", STR_LEN( "4" "\n" ) );
		
		p7::symlink( "v/main/v/v/v/unlock", "accept" );
		
		p7::link( "/gui/new/defaultkeys", "v/defaultkeys/view" );
	}
	
	
	static void run( p7::fd_t buffer, p7::fd_t output )
	{
		setsid();
		
		p7::ioctl( p7::open( "tty", p7::o_rdwr ), TIOCSCTTY, NULL );
		
		const char* gate = "v/main/v/v/v/gate";
		
		char c;
		
		// reading gate blocks
		p7::read( p7::open( gate, p7::o_rdonly ), &c, 1 );
		
		p7::ftruncate( output, 0 );
		
		p7::pump( buffer, output );
		
		p7::_exit( p7::exit_success );
	}
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		if ( *args == NULL )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Usage: buffer input-file\n" ) );
			
			return 1;
		}
		
		const char* input_file = args[ 0 ];
		
		if ( input_file[0] == '-' && input_file[1] == '\0' )
		{
			input_file = "/dev/fd/0";
		}
		
		if ( in_place )
		{
			output_file = input_file;
		}
		
		if ( title == NULL )
		{
			title = input_file;
		}
		
		n::owned< p7::fd_t > cwd = p7::open( ".", p7::o_rdonly | p7::o_directory );
		
		n::owned< p7::fd_t > input = p7::open( input_file, p7::o_rdonly );
		
		make_window( title );
		
		const char* text = "v/main/v/v/v/text";
		const char* lock = "v/main/v/v/v/lock";
		
		p7::pump( input, p7::open( text, p7::o_wronly | p7::o_trunc ) );
		
		p7::close( input );
		
		p7::utime( lock );
		
		n::owned< p7::fd_t > buffer = p7::open( text, p7::o_rdonly );
		
		n::owned< p7::fd_t > output = p7::openat( cwd, output_file, p7::o_wronly | p7::o_creat );
		
		p7::pid_t pid = p7::pid_t();
		
		if ( !should_wait )
		{
			fork_and_exit( p7::exit_success );
		}
		else
		{
			pid = p7::vfork();
		}
		
		if ( pid == 0 )
		{
			try
			{
				// Doesn't return
				run( buffer, output );
			}
			catch ( ... )
			{
				abort();
			}
		}
		
		return n::convert< p7::exit_t >( p7::wait() );
	}
	
}
