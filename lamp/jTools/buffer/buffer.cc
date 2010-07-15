/*	=========
 *	buffer.cc
 *	=========
 */

// Standard C/C++
#include <cstddef>
#include <cstring>

// Standard C
#include <stdlib.h>

// Iota
#include "iota/strings.hh"

// Lamp
#include "lamp/fork_and_exit.h"

// poseven
#include "poseven/extras/pump.hh"
#include "poseven/extras/spew.hh"
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
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	namespace o = orion;
	
	
	static void make_window( const char* title )
	{
		p7::chdir( "/new/port" );
		
		p7::spew( p7::open( "title", p7::o_wronly | p7::o_trunc | p7::o_binary ),
		          title,
		          std::strlen( title ) );
		
		const short width  = 2 * 4 +  6 * 80 + 15;
		const short height = 2 * 4 + 11 * 24;
		
		p7::spew( "size", STR_LEN( "503x272" "\n" ) );
		
		p7::utime( "ref" );
		
		p7::spew( "ref/text-font", STR_LEN( "4" "\n" ) );
		p7::spew( "ref/text-size", STR_LEN( "9" "\n" ) );
		
		p7::link( "/new/stack",       "view"          );
		p7::link( "/new/scrollframe", "view/main"     );
		p7::link( "/new/frame",       "view/main/v"   );
		p7::link( "/new/textedit",    "view/main/v/v" );
		
		p7::symlink( "v/v", "view/main/target" );
		
		p7::spew( "view/main/vertical", STR_LEN( "1" "\n" ) );
		p7::spew( "view/main/v/padding", STR_LEN( "4" "\n" ) );
		
		p7::symlink( "view/main/v/v/gate", "accept" );
		
		p7::link( "/new/defaultkeys", "view/defaultkeys" );
	}
	
	
	static void run( p7::fd_t buffer, p7::fd_t output )
	{
		setsid();
		
		p7::ioctl( p7::open( "tty", p7::o_rdwr ), TIOCSCTTY, NULL );
		
		const char* gate = "view/main/v/v/gate";
		
		char c;
		
		// reading gate blocks
		p7::read( p7::open( gate, p7::o_rdonly ), &c, 1 );
		
		p7::ftruncate( output, 0 );
		
		p7::pump( buffer, output );
		
		p7::_exit( p7::exit_success );
	}
	
	int Main( int argc, char** argv )
	{
		bool should_wait = false;
		
		bool in_place = false;
		
		const char* title = NULL;
		
		const char* output_file = "/dev/null";
		
		o::bind_option_to_variable( "-o", output_file );
		o::bind_option_to_variable( "-t", title       );
		o::bind_option_to_variable( "-w", should_wait );
		
		o::bind_option_to_variable( "--in-place", in_place );
		
		o::alias_option( "-o", "--out"   );
		o::alias_option( "-t", "--title" );
		o::alias_option( "-w", "--wait"  );
		
		o::get_options( argc, argv );
		
		char const *const *freeArgs = o::free_arguments();
		
		if ( *freeArgs == NULL )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Usage: buffer input-file\n" ) );
			
			return 1;
		}
		
		const char* input_file = freeArgs[ 0 ];
		
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
		
		p7::pump( input, p7::open( "view/main/v/v/text", p7::o_wronly | p7::o_trunc ) );
		
		p7::close( input );
		
		p7::utime( "view/main/v/v/interlock" );
		
		n::owned< p7::fd_t > buffer = p7::open( "view/main/v/v/text", p7::o_rdonly );
		
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

