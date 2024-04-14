/*
	tcpcat.cc
	---------
*/

// poseven
#include "poseven/bundles/inet.hh"
#include "poseven/extras/pump.hh"
#include "poseven/functions/write.hh"

// Orion
#include "Orion/Main.hh"


#define PROGRAM  "tcpcat"

#define USAGE  "usage: " PROGRAM " <host> <port>\n"

#define STR_LEN( s )  "" s, (sizeof s - 1)


namespace tool
{

namespace p7 = poseven;


int Main( int argc, char** argv )
{
	if ( argc != 3 )
	{
		p7::write( p7::stderr_fileno, STR_LEN( USAGE ) );
		
		return 1;
	}
	
	const char* hostname = argv[ 1 ];
	const char* port_str = argv[ 2 ];
	
	p7::pump( p7::connect( hostname, port_str ), p7::stdout_fileno );
	
	return 0;
}

}
