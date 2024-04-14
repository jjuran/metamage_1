/*
	tcpclient.cc
	------------
*/

// poseven
#include "poseven/bundles/inet.hh"
#include "poseven/functions/dup2.hh"
#include "poseven/functions/execvp.hh"
#include "poseven/functions/write.hh"

// Orion
#include "Orion/Main.hh"


#define PROGRAM  "tcpclient"

#define USAGE  "usage: " PROGRAM " <host> <port> <program argv>\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)


namespace tool
{

namespace p7 = poseven;


static
void Connect( const char* hostname, const char* port_str )
{
	const p7::fd_t tcp_in  = p7::fd_t( 6 );
	const p7::fd_t tcp_out = p7::fd_t( 7 );
	
	p7::dup2( p7::connect( hostname, port_str ), tcp_in );
	
	p7::dup2( tcp_in, tcp_out );
}

int Main( int argc, char** argv )
{
	if ( argc < 4 )
	{
		p7::write( p7::stderr_fileno, STR_LEN( USAGE ) );
		
		return 1;
	}
	
	const char* hostname = argv[ 1 ];
	const char* port_str = argv[ 2 ];
	
	char** program_argv = argv + 3;
	
	Connect( hostname, port_str );
	
	p7::execvp( program_argv );
	
	// Not reached
	return 0;
}

}
