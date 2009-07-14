/*	=========
 *	buffer.cc
 *	=========
 */

// Standard C/C++
#include <cstddef>
#include <cstring>

// Iota
#include "iota/strings.hh"

// POSeven
#include "POSeven/extras/pump.hh"
#include "POSeven/extras/spew.hh"
#include "POSeven/functions/chdir.hh"
#include "POSeven/functions/execvp.hh"
#include "POSeven/functions/ioctl.hh"
#include "POSeven/functions/link.hh"
#include "POSeven/functions/openat.hh"
#include "POSeven/functions/symlink.hh"
#include "POSeven/functions/utime.hh"
#include "POSeven/functions/vfork.hh"
#include "POSeven/functions/wait.hh"
#include "POSeven/functions/write.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


namespace tool
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	namespace O = Orion;
	
	
	static void make_window( const char* title )
	{
		p7::chdir( "/new/window" );
		
		p7::spew( p7::open( "title", p7::o_wronly | p7::o_trunc | p7::o_binary ),
		          title,
		          std::strlen( title ) );
		
		const short width  = 2 * 4 +  6 * 80 + 15;
		const short height = 2 * 4 + 11 * 24;
		
		p7::spew( "size", STR_LEN( "503x272" "\n" ) );
		
		p7::utime( "ref" );
		
		p7::spew( "ref/text-font", STR_LEN( "4" "\n" ) );
		p7::spew( "ref/text-size", STR_LEN( "9" "\n" ) );
		
		p7::link( "/new/scrollframe", "view"     );
		p7::link( "/new/frame",       "view/v"   );
		p7::link( "/new/textedit",    "view/v/v" );
		
		p7::symlink( "v/v", "view/target" );
		
		p7::spew( "view/vertical", STR_LEN( "1" "\n" ) );
		p7::spew( "view/v/padding", STR_LEN( "4" "\n" ) );
	}
	
	
	int Main( int argc, iota::argv_t argv )
	{
		bool should_wait = false;
		
		bool in_place = false;
		
		const char* title = NULL;
		
		const char* output_file = "/dev/null";
		
		O::BindOption( "-o", output_file );
		O::BindOption( "-t", title       );
		O::BindOption( "-w", should_wait );
		
		O::BindOption( "--in-place", in_place );
		
		O::AliasOption( "-o", "--out"   );
		O::AliasOption( "-t", "--title" );
		O::AliasOption( "-w", "--wait"  );
		
		O::GetOptions( argc, argv );
		
		char const *const *freeArgs = O::FreeArguments();
		
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
		
		NN::Owned< p7::fd_t > cwd = p7::open( ".", p7::o_rdonly | p7::o_directory );
		
		NN::Owned< p7::fd_t > input = p7::open( input_file, p7::o_rdonly );
		
		make_window( title );
		
		p7::pump( input, p7::open( "view/v/v/text", p7::o_wronly | p7::o_trunc ) );
		
		p7::close( input );
		
		p7::utime( "view/v/v/interlock" );
		
		NN::Owned< p7::fd_t > buffer = p7::open( "view/v/v/text", p7::o_rdonly );
		
		NN::Owned< p7::fd_t > output = p7::openat( cwd, output_file, p7::o_wronly | p7::o_creat | p7::o_trunc_lazy );
		
		p7::pid_t pid = p7::vfork();
		
		if ( pid == 0 )
		{
			// New child, so we're not a process group leader
			
			setsid();
			
			p7::ioctl( p7::open( "tty", p7::o_rdwr ), TIOCSCTTY, NULL );
			
			dup2( buffer, 0 );
			dup2( output, 1 );
			
			const char* gate = "view/v/v/gate";
			
			const char* window_argv[] = { "/bin/cat", gate, "-", NULL };
			
			p7::execvp( window_argv );
		}
		
		if ( should_wait )
		{
			return NN::Convert< p7::exit_t >( p7::wait() );
		}
		
		return p7::exit_success;
	}
	
}

