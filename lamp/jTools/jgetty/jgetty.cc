/*	=========
 *	jgetty.cc
 *	=========
 */

// Standard C/C++
#include <cstring>

// Standard C
#include "stdlib.h"

// Iota
#include "iota/strings.hh"

// poseven
#include "poseven/extras/spew.hh"
#include "poseven/functions/chdir.hh"
#include "poseven/functions/dup2.hh"
#include "poseven/functions/execv.hh"
#include "poseven/functions/ioctl.hh"
#include "poseven/functions/link.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/rename.hh"
#include "poseven/functions/setsid.hh"
#include "poseven/functions/symlink.hh"
#include "poseven/functions/utime.hh"
#include "poseven/functions/vfork.hh"
#include "poseven/functions/wait.hh"


namespace n = nucleus;
namespace p7 = poseven;


namespace tool
{
	
	static const char* getcwd()
	{
		static char buffer[] = "/sys/window/abcd3210";
		
		if ( ::getcwd( buffer, sizeof buffer ) == NULL )
		{
			p7::throw_errno( errno );
		}
		
		return buffer;
	}
	
}


int main( int argc, char const *const argv[] )
{
	p7::chdir( "/new/port" );
	
	setenv( "WINDOW", tool::getcwd(), true );
	
	const short width  = 2 * 4 +  6 * 80 + 15;
	const short height = 2 * 4 + 11 * 24;
	
	p7::spew( "size", STR_LEN( "503x272" "\n" ) );
	
	p7::utime( "ref" );
	
	p7::spew( "ref/text-font", STR_LEN( "4" "\n" ) );
	p7::spew( "ref/text-size", STR_LEN( "9" "\n" ) );
	
	p7::link( "/new/scrollframe", "view"     );
	p7::link( "/new/frame",       "view/v"   );
	p7::link( "/new/console",     "view/v/v" );
	
	p7::symlink( "v/v", "view/target" );
	
	p7::spew( "view/vertical", STR_LEN( "1" "\n" ) );
	p7::spew( "view/v/padding", STR_LEN( "4" "\n" ) );
	
	p7::rename( "view/v/v/tty", "tty" );
	
	n::owned< p7::fd_t > tty = p7::open( "tty", p7::o_rdwr );
	
	p7::dup2( tty, p7::stdin_fileno  );
	p7::dup2( tty, p7::stdout_fileno );
	p7::dup2( tty, p7::stderr_fileno );
	
	if ( tty.get() > p7::stderr_fileno )
	{
		p7::close( tty );
	}
	
	p7::pid_t sid = p7::setsid();  // New session
	
	p7::ioctl( p7::stdin_fileno, TIOCSCTTY, NULL );  // Reattach to terminal
	
	if ( const char* name = ttyname( p7::stdin_fileno ) )
	{
		p7::spew( p7::open( "title", p7::o_wronly | p7::o_trunc | p7::o_binary ),
		          name,
		          std::strlen( name ) );
	}
	
	p7::pid_t pid = p7::vfork();
	
	if ( pid == 0 )
	{
		p7::chdir( "/" );
		
		char* login_argv[] = { "/bin/login", NULL };
		
		p7::execv( login_argv[0], login_argv );
	}
	
	p7::wait_t wait_status = p7::wait();
	
	// If the child exited unsuccessfully and the terminal is still connected,
	// keep the window around.
	if ( wait_status != 0  &&  ::write( p7::stdout_fileno, "", 0 ) == 0 )
	{
		// Reset ourselves as the foreground process group of the terminal,
		// so we'll get SIGHUP when the window is closed.
		int pgrpset = tcsetpgrp( p7::stdin_fileno, sid );
		
		p7::spew( "title", STR_LEN( "(Done)" "\n" ) );
		
		// Loop so resizing the window doesn't kill us (via SIGWINCH).
		while ( true )
		{
			pause();
		}
	}
	
	return 0;
}

