/*	=========
 *	buffer.cc
 *	=========
 */

// Standard C/C++
#include <cstddef>

// Iota
#include "iota/strings.hh"

// POSeven
#include "POSeven/extras/pump.hh"
#include "POSeven/extras/spew.hh"
#include "POSeven/functions/execvp.hh"
#include "POSeven/functions/ioctl.hh"
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
		p7::throw_posix_result( ::chdir( "/new/window" ) );
		
		std::string title_data = title;
		
		title_data += '\n';
		
		p7::spew( "title", title_data );
		
		p7::throw_posix_result( ::link( "/new/scrollframe", "view"     ) );
		p7::throw_posix_result( ::link( "/new/frame",       "view/v"   ) );
		p7::throw_posix_result( ::link( "/new/textedit",    "view/v/v" ) );
		
		p7::symlink( "v/v", "view/target" );
		
		p7::spew( "view/vertical", "1" "\n" );
		p7::spew( "view/v/margin", "4" "\n" );
		
		p7::utime( "ref" );
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
			
			const char* window_argv[] = { "/bin/cat", NULL };
			
			p7::execvp( window_argv );
		}
		
		if ( should_wait )
		{
			return NN::Convert< p7::exit_t >( p7::wait() );
		}
		
		return p7::exit_success;
	}
	
}

