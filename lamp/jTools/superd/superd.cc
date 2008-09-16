/*	=========
 *	superd.cc
 *	=========
 */

// Standard C
#include <errno.h>

// Standard C/C++
#include <cstring>

// POSIX
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

// Iota
#include "iota/strings.hh"

// POSeven
#include "POSeven/FileDescriptor.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
	static int ForkCommand( int client, iota::argv_t argv )
	{
		int pid = vfork();
		
		if ( pid == 0 )
		{
			// We're the child
			dup2( client, 0 );  // Input from client
			dup2( client, 1 );  // Output to client
			//dup2( client, 2 );  // Error inherited
			
			int result = close( client );  // Extraneous fd
			
			result = execv( argv[ 0 ], argv );
			
			std::string message = "superd: execv( ";
			
			message += argv[ 0 ];
			message += " ) failed";
			
			std::perror( message.c_str() );
			
			_exit( 1 );  // Use _exit() to exit a forked but not exec'ed process.
		}
		
		return pid;
	}
	
	static void ServiceClient( int client, iota::argv_t argv )
	{
		int pid = ForkCommand( client, argv );
		
		int result = close( client );
		
		int stat;
		
		pid = waitpid( pid, &stat, 0 );
	}
	
	static void WaitForClients( int listener, iota::argv_t argv )
	{
		struct sockaddr_in from;
		socklen_t len = sizeof from;
		
		int failures = 0;
		
		while ( true )
		{
			// This blocks and yields to other threads
			int client = accept( listener, (sockaddr*)&from, &len );
			
			if ( client == -1 )
			{
				std::perror( "superd: accept()" );
				
				if ( ++failures >= 3 )
				{
					break;
				}
				
				continue;
			}
			
			ServiceClient( client, argv );
		}
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		if ( argc <= 2 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Usage: superd port command\n" ) );
			return 1;
		}
		
		unsigned short port = std::atoi( argv[ 1 ] );
		
		const char* command = argv[ 2 ];
		
		struct in_addr ip = { 0 };
		
		p7::write( p7::stdout_fileno, STR_LEN( "Daemon starting up..." ) );
		
		int listener = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
		
		if ( listener == -1 )
		{
			std::perror( "superd: socket()" );
			return 1;
		}
		
		struct sockaddr_in inetAddress;
		
		inetAddress.sin_family = AF_INET;
		inetAddress.sin_port = htons( port );
		inetAddress.sin_addr = ip;
		
		int result = bind( listener, (const sockaddr*)&inetAddress, sizeof (sockaddr_in) );
		
		if ( result == -1 )
		{
			std::perror( "superd: bind()" );
			return 1;
		}
		
		result = listen( listener, 3 );
		
		if ( result == -1 )
		{
			std::perror( "superd: listen()" );
			return 1;
		}
		
		p7::write( p7::stdout_fileno, STR_LEN( " done.\n" ) );
		
		WaitForClients( listener, argv + 2 );
		
		result = close( listener );
		
		return 0;
	}
	
}

