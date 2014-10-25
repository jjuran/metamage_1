/*
	channel.cc
	----------
*/

#include "unet/channel.hh"

// POSIX
#include <unistd.h>

// gear
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/string.hh"

// poseven
#include "poseven/functions/dup2.hh"
#include "poseven/functions/execvp.hh"
#include "poseven/functions/socketpair.hh"
#include "poseven/types/errno_t.hh"


#ifdef __RELIX__
#define FORK() ::vfork()
#else
#define FORK() ::fork()
#endif


namespace unet
{
	
	using namespace poseven;
	
	
	coprocess_box enchannel( int in, int out, const char* argv[] )
	{
		/*
			conn is our (clear) connection P to the peer.
			We're going to create a new socket pair with ends A and B.
			End A will be returned to the caller in place of P.
			End B will be duped to 0 and 1 and P will be duped to 6 and 7
			before execing the channeler.
		*/
		
		if ( argv[ 0 ] == NULL )
		{
			return 0;
		}
		
		const char** argp;
		
		for ( argp = argv + 1;  *argp != NULL;  ++argp )
		{
		}
		
		const int argc = argp - argv;
		
		// e.g. uxor "" "" "" ""
		
		if ( argc < 1 + 4 )
		{
			return -1;
		}
		
		for ( int i = 4;  i > 0;  --i )
		{
			if ( **--argp != '\0' )
			{
				return -1;
			}
		}
		
		// Create the clear channel
		
		fd_pair fds = socketpair( pf_local, sock_stream );
		
		::pid_t child = throw_posix_result( FORK() );
		
		if ( child == 0 )
		{
			::close( fds[ 0 ] );
			
			plus::string clear_fd = gear::inscribe_decimal( fds[ 1 ] );
			
			plus::string muxed_in_fd  = gear::inscribe_decimal( in  );
			plus::string muxed_out_fd = gear::inscribe_decimal( out );
			
			*argp++ = clear_fd.c_str();
			*argp++ = clear_fd.c_str();
			
			*argp++ = muxed_in_fd.c_str();
			*argp++ = muxed_out_fd.c_str();
			
			// Run the channeler
			
			execvp( argv );
		}
		
		coprocess_box result( child );
		
		dup2( fds[ 0 ], in  );
		dup2( fds[ 0 ], out );
		
		return result;
	}
	
}
