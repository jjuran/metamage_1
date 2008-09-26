/*	============
 *	tcpclient.cc
 *	============
 */

// Standard C/C++
#include <cerrno>
#include <cstdlib>

// Standard C++
#include <string>

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
	
	static NN::Owned< p7::fd_t > Connect( const char* hostname, const char* port_str )
	{
		NN::Owned< p7::fd_t > result = p7::socket( p7::pf_inet, p7::sock_stream );
		
		p7::in_port_t port = p7::in_port_t( std::atoi( port_str ) );
		
		p7::in_addr_t addr = ResolveHostname( hostname );
		
		p7::connect( result, addr, port );
		
		return result;
	}
	
	int Main( int argc, iota::argv_t argv )
	{
		if ( argc < 4 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Usage:  tcpclient <host> <port> <program argv>\n" ) );
			
			return 1;
		}
		
		const char* hostname = argv[1];
		const char* port_str = argv[2];
		
		iota::argp_t program_argv = argv + 3;
		
		NN::Owned< p7::fd_t > sock = Connect( hostname, port_str );
		
		dup2( sock, 6 );
		dup2( sock, 7 );
		
		execvp( program_argv[0], program_argv );
		
		_exit( errno == ENOENT ? 127 : 126 );
		
		return 0;
	}
	
}

