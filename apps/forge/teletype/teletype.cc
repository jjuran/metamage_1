/*
	teletype.cc
	-----------
*/

// Standard C
#include <signal.h>
#include <stdlib.h>
#include <string.h>

// POSIX
#include <sys/ttycom.h>

// Relix
#include "relix/fork_and_exit.h"

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


#define STR_LEN( s )  "" s, (sizeof s - 1)


namespace tool
{
	
	static const char* getcwd()
	{
		static char buffer[] = "/gui/port/abcd3210";
		
		if ( ::getcwd( buffer, sizeof buffer ) == NULL )
		{
			p7::throw_errno( errno );
		}
		
		return buffer;
	}
	
}


int main( int argc, char const *const argv[] )
{
	p7::chdir( "/gui/new/port" );
	
	setenv( "PORT", tool::getcwd(), true );
	
	const short width  = 2 * 4 +  6 * 80 + 15;
	const short height = 2 * 4 + 11 * 24;
	
	p7::spew( "size", STR_LEN( "503x272" "\n" ) );
	
	p7::spew( p7::open( ".~title", p7::o_wronly | p7::o_trunc ),
	          STR_LEN( "teletype" ) );
	
	p7::utime( "window" );
	
	p7::spew( "w/text-font", STR_LEN( "4" "\n" ) );
	p7::spew( "w/text-size", STR_LEN( "9" "\n" ) );
	
	p7::link( "/gui/new/scrollframe", "view"     );
	p7::link( "/gui/new/frame",       "v/view"   );
	p7::link( "/gui/new/console",     "v/v/view" );
	
	p7::symlink( "v/v", "v/target" );
	
	p7::spew( "v/vertical", STR_LEN( "1" "\n" ) );
	p7::spew( "v/v/padding", STR_LEN( "4" "\n" ) );
	
	p7::rename( "v/v/v/tty", "tty" );
	
	n::owned< p7::fd_t > tty = p7::open( "tty", p7::o_rdwr );
	
	p7::dup2( tty, p7::stdin_fileno  );
	p7::dup2( tty, p7::stdout_fileno );
	p7::dup2( tty, p7::stderr_fileno );
	
	if ( tty.get() > p7::stderr_fileno )
	{
		p7::close( tty );
	}
	
	if ( getsid( 0 ) != 1 )
	{
		// Ensure that the caller doesn't wait for us
		// (Also ensures we are not a process group leader)
		fork_and_exit( 0 );
	}
	
	p7::pid_t sid = p7::setsid();  // New session
	
	// Restore default SIGHUP handling in case we were invoked from daemonize
	
	struct sigaction action = { SIG_DFL, 0, 0 };
	
	sigaction( SIGHUP, &action, NULL );
	
	p7::ioctl( p7::stdin_fileno, TIOCSCTTY, NULL );  // Reattach to terminal
	
	if ( const char* name = ttyname( STDIN_FILENO ) )
	{
		p7::spew( p7::open( ".~title", p7::o_wronly | p7::o_trunc ),
		          name,
		          std::strlen( name ) );
	}
	
	p7::pid_t pid = p7::vfork();
	
	if ( pid == 0 )
	{
		p7::chdir( "/" );
		
		char* login_argv[] = { "/bin/login", NULL };
		
		const bool use_login = argc < 2;
		
		const char* const* next_argv = use_login ? login_argv
		                                         : argv + 1;
		
		p7::execv( next_argv[0], next_argv );
	}
	
	p7::wait_t wait_status = p7::wait();
	
	// If the child exited unsuccessfully and the terminal is still connected,
	// keep the window around.
	if ( wait_status != 0 )
	{
		// Reset ourselves as the foreground process group of the terminal,
		// so we'll get SIGHUP when the window is closed.
		int pgrpset = tcsetpgrp( STDIN_FILENO, sid );
		
		p7::spew( "title", STR_LEN( "(Done)" "\n" ) );
		
		// Loop so resizing the window doesn't kill us (via SIGWINCH).
		while ( true )
		{
			pause();
		}
	}
	
	return 0;
}
