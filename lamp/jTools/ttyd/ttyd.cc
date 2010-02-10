/*	=======
 *	ttyd.cc
 *	=======
 */

// poseven
#include "poseven/functions/clearenv.hh"
#include "poseven/functions/ioctl.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/select.hh"
#include "poseven/functions/setsid.hh"
#include "poseven/functions/sigaction.hh"
#include "poseven/functions/execv.hh"
#include "poseven/functions/vfork.hh"
#include "poseven/functions/write.hh"
#include "poseven/functions/_exit.hh"

// Orion
#include "Orion/Main.hh"


extern "C" int ttypair( int filedes[ 2 ] );


namespace tool
{
	
	namespace p7 = poseven;
	
	
	static void sigchld_handler( int )
	{
		p7::_exit( p7::exit_success );
	}
	
	static p7::fd_t Spawn()
	{
		int fds[2];
		
		int paired = ttypair( fds );
		
		int master = fds[0];
		int slave  = fds[1];
		
		p7::pid_t pid = p7::vfork();
		
		if ( pid == 0 )
		{
			close( master );
			
			dup2( slave, STDIN_FILENO  );
			dup2( slave, STDOUT_FILENO );
			dup2( slave, STDERR_FILENO );
			
			if ( slave > STDERR_FILENO )
			{
				close( slave );
			}
			
			p7::pid_t sid = p7::setsid();  // New session
			
			p7::ioctl( p7::stdin_fileno, TIOCSCTTY, NULL );  // Reattach to terminal
			
			p7::clearenv();
			
			const char* login_argv[] = { "/bin/login", NULL };
			
			p7::execv( login_argv );
		}
		
		close( slave );
		
		dup2( STDOUT_FILENO, STDERR_FILENO );
		
		return p7::fd_t( master );
	}
	
	static void Serve( p7::fd_t master )
	{
		const std::size_t block_size = 4096;
		
		char buffer[ block_size ];
		
		fd_set fds;
		
		FD_ZERO( &fds );
		
		FD_SET( STDIN_FILENO, &fds );
		FD_SET( master,       &fds );
		
		const p7::fd_t max_fd = master;
		
		while ( true )
		{
			fd_set readfds = fds;
			
			unsigned selected = p7::select( max_fd + 1, &readfds, NULL, NULL, NULL );
			
			if ( FD_ISSET( STDIN_FILENO, &readfds ) )
			{
				if ( int bytes = p7::read( p7::stdin_fileno, buffer, block_size ) )
				{
					p7::write( master, buffer, bytes );
				}
				else
				{
					break;
				}
			}
			
			if ( FD_ISSET( master, &readfds ) )
			{
				if ( int bytes = p7::read( master, buffer, block_size ) )
				{
					p7::write( p7::stdout_fileno, buffer, bytes );
				}
				else
				{
					break;
				}
			}
		}
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		p7::pid_t sid = p7::setsid();  // New session
		
		p7::sigaction( p7::sigchld, sigchld_handler );
		
		p7::fd_t master = Spawn();
		
		Serve( master );
		
		return p7::exit_success;
	}
	
}

