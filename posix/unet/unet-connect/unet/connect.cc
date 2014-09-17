/*
	connect.cc
	----------
*/

#include "unet/connect.hh"

// POSIX
#include <unistd.h>

// Standard C
#include <errno.h>

// pass_fd
#include "unet/pass_fd.hh"

// poseven
#include "poseven/functions/close.hh"
#include "poseven/functions/dup2.hh"
#include "poseven/functions/execvp.hh"
#include "poseven/functions/socketpair.hh"
#include "poseven/functions/waitpid.hh"


#ifdef __RELIX__
#define FORK() ::vfork()
#else
#define FORK() ::fork()
#endif


namespace unet
{
	
	namespace n = nucleus;
	
	using namespace poseven;
	
	
	static n::owned< fd_t > recv_fd_t( fd_t socket_fd )
	{
		int fd = throw_posix_result( unet::recv_fd( socket_fd ) );
		
		return n::owned< fd_t >::seize( fd_t( fd ) );
	}
	
	connection_box connect( const char* argv[] )
	{
		if ( argv[ 0 ] == NULL )
		{
			const int protocol_in  = 6;
			const int protocol_out = 7;
			
			return connection_box( argv, protocol_in, protocol_out );
		}
		
		// Create the unet control channel
		
		fd_pair fds = socketpair( pf_local, sock_stream );
		
		::pid_t child = throw_posix_result( FORK() );
		
		if ( child == 0 )
		{
			// Dup the control socket to stdout
			
			dup2( fds[ 1 ], stdout_fileno );
			
			::close( fds[ 0 ] );
			::close( fds[ 1 ] );
			
			// Run the connector
			
			execvp( argv );
		}
		
		::close( fds.release( 1 ) );
		
		// Get server fds over control channel
		
		n::owned< fd_t > in  = recv_fd_t( fds[ 0 ] );
		n::owned< fd_t > out = recv_fd_t( fds[ 0 ] );
		
		char c;
		
		ssize_t n_read = read( fds[ 0 ], &c, sizeof c );
		
		if ( n_read == 0 )
		{
			// Wait for connector
		
			if ( wait_t w = waitpid( poseven::pid_t( child ) ) )
			{
				throw_errno( EIO );
			}
		}
		
		return connection_box( argv, in.release(), out.release() );
	}
	
}
