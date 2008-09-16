/*	=======
 *	ttyd.cc
 *	=======
 */

// POSIX
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>

// POSeven
#include "POSeven/Errno.hh"
#include "POSeven/FileDescriptor.hh"

// Orion
#include "Orion/Main.hh"


extern "C" int ttypair( int filedes[ 2 ] );


namespace tool
{
	
	namespace p7 = poseven;
	
	
	static void sigchld_handler( int )
	{
		_exit( 0 );
	}
	
	static int Spawn()
	{
		int fds[2];
		
		int paired = ttypair( fds );
		
		int master = fds[0];
		int slave  = fds[1];
		
		pid_t pid = vfork();
		
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
			
			int sid = setsid();  // New session
			
			if ( sid < 0 )
			{
				_exit( EXIT_FAILURE );
			}
			
			int result = ioctl( STDIN_FILENO, TIOCSCTTY, NULL );  // Reattach to terminal
			
			if ( result < 0 )
			{
				_exit( EXIT_FAILURE );
			}
			
			const char* login_argv[] = { "/bin/login", NULL };
			
			execv( login_argv[0], login_argv );
			
			_exit( 127 );
		}
		
		close( slave );
		
		dup2( STDOUT_FILENO, STDERR_FILENO );
		
		return master;
	}
	
	static void Serve( p7::fd_t master )
	{
		const std::size_t block_size = 4096;
		
		char buffer[ block_size ];
		
		fd_set fds;
		
		FD_ZERO( &fds );
		
		FD_SET( STDIN_FILENO, &fds );
		FD_SET( master,       &fds );
		
		const int max_fd = master;
		
		while ( true )
		{
			fd_set readfds = fds;
			
			int selected = p7::throw_posix_result( select( max_fd + 1, &readfds, NULL, NULL, NULL ) );
			
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
		int sid = setsid();  // New session
		
		if ( sid < 0 )
		{
			return 1;
		}
		
		signal( SIGCHLD, sigchld_handler );
		
		int master = Spawn();
		
		Serve( p7::fd_t( master ) );
		
		return 0;
	}
	
}

namespace Orion
{
	
	int Main( int argc, iota::argv_t argv )
	{
		return tool::Main( argc, argv );
	}
	
}

