/*	=========
 *	daemon.cc
 *	=========
 */

// Standard C
#include <errno.h>

// Standard C/C++
#include <cstring>

// POSIX
#include "sys/socket.h"
#include "sys/wait.h"
#include "unistd.h"

// Nitrogen / Carbon support
#include "Nitrogen/OpenTransportProviders.h"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace N = Nitrogen;
namespace O = Orion;


static int ForkCommand( int client, char *const argv[] )
{
	int pid = fork();
	
	if ( is_child( pid ) )
	{
		dup2( client, 0 );
		dup2( client, 1 );
		//dup2( client, 2 );
		
		int result = close( client );
		
		std::string command = "/bin/";
		
		command += argv[ 0 ];
		
		result = execv( command.c_str(), argv );
		
		if ( result == -1 )
		{
			_exit( 1 );  // Use _exit() to exit a forked but not exec'ed process.
			Io::Err << "execv( " << command << " ) failed\n";
		}
	}
	
	return pid;
}

static void ServiceClient( int client, char *const argv[] )
{
	int pid = ForkCommand( client, argv );
	
	int result = close( client );
	
	int stat;
	
	pid = waitpid( pid, &stat, 0 );
}

static void WaitForClients( int listener, char *const argv[] )
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
			Io::Err << "accept() failed:  " << errno << "\n";
			
			if ( ++failures >= 3 )
			{
				break;
			}
			
			continue;
		}
		
		ServiceClient( client, argv );
	}
}

int O::Main( int argc, char const *const argv[] )
{
	if ( argc <= 2 )
	{
		Io::Err << "Usage: daemon port command\n";
		return 1;
	}
	
	unsigned short port = std::atoi( argv[ 1 ] );
	
	const char* command = argv[ 2 ];
	
	N::InetHost ip( kOTAnyInetAddress );
	
	Io::Out << "Daemon starting up...";
	//ForceUpdate();
	
	int listener = socket( PF_INET, SOCK_STREAM, INET_TCP );
	
	if ( listener == -1 )
	{
		Io::Err << "socket() failed:  " << errno << "\n";
		return 1;
	}
	
	struct sockaddr_in inetAddress;
	
	inetAddress.sin_family = AF_INET;
	inetAddress.sin_port = port;
	inetAddress.sin_addr = ip;
	
	int result = bind( listener, (const sockaddr*)&inetAddress, sizeof (sockaddr_in) );
	
	if ( result == -1 )
	{
		Io::Err << "bind() failed:  " << errno << "\n";
		return 1;
	}
	
	result = listen( listener, 3 );
	
	if ( result == -1 )
	{
		Io::Err << "listen() failed:  " << errno << "\n";
		return 1;
	}
	
	Io::Out << " done.\n";
	
	WaitForClients( listener, const_cast< char* const* >( argv ) + 2 );
	
	result = close( listener );
	
	return 0;
}

