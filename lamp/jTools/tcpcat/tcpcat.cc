/*	=========
 *	tcpcat.cc
 *	=========
 */

// Standard C++
#include <string>

// Standard C/C++
#include <cerrno>
#include <cstdlib>

// POSIX
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>

// Iota
#include "iota/strings.hh"

// Nucleus
#include "Nucleus/Convert.h"

// POSeven
#include "POSeven/bundles/inet.hh"
#include "POSeven/extras/pump.hh"
#include "POSeven/functions/socket.hh"
#include "POSeven/functions/write.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	static p7::in_addr_t ResolveHostname( const char* hostname )
	{
		hostent* hosts = gethostbyname( hostname );
		
		if ( !hosts || h_errno )
		{
			std::string message = "Domain name lookup failed: ";
			
			message += NN::Convert< std::string >( h_errno );
			message += "\n";
			
			p7::write( p7::stderr_fileno, message );
			
			throw p7::exit_failure;
		}
		
		in_addr addr = *(in_addr*) hosts->h_addr;
		
		return p7::in_addr_t( addr.s_addr );
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		if ( argc != 3 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Usage:  tcpcat <host> <port>\n" ) );
			
			return 1;
		}
		
		const char* hostname = argv[1];
		const char* port_str = argv[2];
		
		NN::Owned< p7::fd_t > sock = p7::socket( p7::pf_inet, p7::sock_stream );
		
		p7::in_port_t port = p7::in_port_t( std::atoi( port_str ) );
		
		p7::in_addr_t addr = ResolveHostname( hostname );
		
		p7::connect( sock, addr, port );
		
		p7::pump( sock, p7::stdout_fileno );
		
		return 0;
	}
	
}

