/*	=========
 *	superd.cc
 *	=========
 */

// Standard C
#include <errno.h>

// Standard C/C++
#include <cstring>

// Iota
#include "iota/strings.hh"

// POSeven
#include "POSeven/bundles/inet.hh"
#include "POSeven/functions/close.hh"
#include "POSeven/functions/dup2.hh"
#include "POSeven/functions/execv.hh"
#include "POSeven/functions/listen.hh"
#include "POSeven/functions/vfork.hh"
#include "POSeven/functions/wait.hh"
#include "POSeven/functions/write.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	static void ServiceClient( p7::fd_t client, iota::argv_t argv )
	{
		p7::pid_t pid = POSEVEN_VFORK();
		
		if ( pid == 0 )
		{
			// We're the child
			p7::dup2( client, p7::stdin_fileno  );  // Input from client
			p7::dup2( client, p7::stdout_fileno );  // Output to client
			//dup2( client, 2 );  // Error inherited
			
			int result = close( client );  // Extraneous fd
			
			p7::execv( argv );
		}
	}
	
	static void WaitForClients( p7::fd_t listener, iota::argv_t argv )
	{
		while ( true )
		{
			// This blocks and yields to other threads
			ServiceClient( p7::accept( listener ), argv );
			
			(void) p7::wait();
		}
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		if ( argc <= 2 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Usage: superd port command\n" ) );
			return 1;
		}
		
		p7::in_port_t port = p7::in_port_t( std::atoi( argv[ 1 ] ) );
		
		const char* command = argv[ 2 ];
		
		p7::write( p7::stdout_fileno, STR_LEN( "Daemon starting up..." ) );
		
		NN::Owned< p7::fd_t > listener = p7::bind( p7::inaddr_any, port );
		
		p7::listen( listener, 3 );
		
		p7::write( p7::stdout_fileno, STR_LEN( " done.\n" ) );
		
		WaitForClients( listener, argv + 2 );
		
		p7::close( listener );
		
		return 0;
	}
	
}

