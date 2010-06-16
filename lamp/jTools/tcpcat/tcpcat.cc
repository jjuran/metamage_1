/*	=========
 *	tcpcat.cc
 *	=========
 */

// Standard C/C++
#include <cerrno>

// POSIX
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>

// Iota
#include "iota/decimal.hh"
#include "iota/strings.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/bundles/inet.hh"
#include "poseven/extras/pump.hh"
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
			plus::var_string message = "Domain name lookup failed: ";
			
			message += iota::inscribe_decimal( h_errno );
			message += "\n";
			
			p7::write( p7::stderr_fileno, message );
			
			throw p7::exit_failure;
		}
		
		in_addr addr = *(in_addr*) hosts->h_addr;
		
		return p7::in_addr_t( addr.s_addr );
	}
	
	int Main( int argc, char** argv )
	{
		if ( argc != 3 )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "Usage:  tcpcat <host> <port>\n" ) );
			
			return 1;
		}
		
		const char* hostname = argv[1];
		const char* port_str = argv[2];
		
		p7::in_port_t port = p7::in_port_t( iota::parse_unsigned_decimal( port_str ) );
		
		p7::in_addr_t addr = ResolveHostname( hostname );
		
		p7::pump( p7::connect( addr, port ),
		          p7::stdout_fileno );
		
		return 0;
	}
	
}

