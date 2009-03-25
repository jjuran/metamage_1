/*	==========
 *	console.cc
 *	==========
 */

// Standard C/C++
#include <cstring>

// POSIX
#include <fcntl.h>
#include <sys/ttycom.h>

// Lamp
#include "lamp/winio.h"

// Iota
#include "iota/strings.hh"

// POSeven
#include "POSeven/Open.hh"
#include "POSeven/functions/execvp.hh"
#include "POSeven/functions/ioctl.hh"
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
	
	
	int Main( int argc, iota::argv_t argv )
	{
		bool should_wait = false;
		
		const char* title = NULL;
		
		const char* device = "/dev/new/console";
		
		O::BindOption( "-d", device      );
		O::BindOption( "-t", title       );
		O::BindOption( "-w", should_wait );
		
		O::AliasOption( "-d", "--dev"   );
		O::AliasOption( "-t", "--title" );
		O::AliasOption( "-w", "--wait"  );
		
		O::GetOptions( argc, argv );
		
		char const *const *freeArgs = O::FreeArguments();
		
		if ( *freeArgs == NULL )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Usage: console command [ arg1 ... argn ]\n" ) );
			
			return 1;
		}
		
		p7::pid_t pid = p7::vfork();
		
		if ( pid == 0 )
		{
			// New child, so we're not a process group leader
			
			setsid();
			
			NN::Owned< p7::fd_t > console = p7::open( device, p7::o_rdwr );
			
			p7::ioctl( console, TIOCSCTTY, NULL );
			
			if ( title != NULL )
			{
				p7::ioctl( console, WIOCSTITLE, title );
			}
			
			dup2( console, 0 );
			dup2( console, 1 );
			dup2( console, 2 );
			
			close( console );
			
			p7::execvp( &freeArgs[ 0 ] );
		}
		
		if ( should_wait )
		{
			return NN::Convert< p7::exit_t >( p7::wait() );
		}
		
		return p7::exit_success;
	}
	
}

