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
#include "iota/decimal.hh"
#include "iota/strings.hh"

// poseven
#include "poseven/bundles/inet.hh"
#include "poseven/functions/dup2.hh"
#include "poseven/functions/execvp.hh"
#include "poseven/functions/socket.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/exit_t.hh"

// Orion
#include "Orion/Main.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
	static p7::in_addr_t ResolveHostname( const char* hostname )
	{
		hostent* hosts = gethostbyname( hostname );
		
		if ( !hosts || h_errno )
		{
			std::string message = "Domain name lookup failed: ";
			
			message += iota::inscribe_decimal( h_errno );
			message += "\n";
			
			p7::write( p7::stderr_fileno, message );
			
			throw p7::exit_failure;
		}
		
		in_addr addr = *(in_addr*) hosts->h_addr;
		
		return p7::in_addr_t( addr.s_addr );
	}
	
	static void Connect( const char* hostname, const char* port_str )
	{
		p7::in_port_t port = p7::in_port_t( std::atoi( port_str ) );
		
		p7::in_addr_t addr = ResolveHostname( hostname );
		
		const p7::fd_t tcp_in  = p7::fd_t( 6 );
		const p7::fd_t tcp_out = p7::fd_t( 7 );
		
		p7::dup2( p7::connect( addr, port ), tcp_in );
		
		p7::dup2( tcp_in, tcp_out );
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
		
		Connect( hostname, port_str );
		
		p7::execvp( program_argv );
		
		// Not reached
		return 0;
	}
	
}

