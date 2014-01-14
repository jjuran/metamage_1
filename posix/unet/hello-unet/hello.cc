/*
	hello.cc
	--------
*/

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>
#include <string.h>

// poseven
#include "poseven/functions/read.hh"
#include "poseven/functions/write.hh"
#include "poseven/types/errno_t.hh"

// unet-connect
#include "unet/connect.hh"


#define PROGRAM "hello-unet"


#define STRLEN( s )  (sizeof "" s - 1)

#define STR_LEN( s )  s, STRLEN( s )


namespace p7 = poseven;


int main( int argc, char** argv )
{
	const char* connector = getenv( "UNET_CONNECT" );
	
	if ( connector == NULL )
	{
		connector = "uloop";
	}
	
	const char* connector_argv[] = { "/bin/sh", "-c", connector, NULL };
	
	try
	{
		unet::connection_box connection = unet::connect( connector_argv );
		
		p7::fd_t in  = p7::fd_t( connection.get_input () );
		p7::fd_t out = p7::fd_t( connection.get_output() );
		
		p7::write( out, STR_LEN( "Hello world\n" ) );
		
		::close( out );
		
		char buffer[ 4096 ];
		
		while ( ssize_t n = p7::read( in, buffer, sizeof buffer ) )
		{
			p7::write( p7::stdout_fileno, buffer, n );
		}
	}
	catch ( const p7::errno_t& err )
	{
		const char* str = strerror( err );
		
		write( STDERR_FILENO, STR_LEN( PROGRAM ": " ) );
		write( STDERR_FILENO, str, strlen( str ) );
		write( STDERR_FILENO, STR_LEN( "\n" ) );
	}
	
	return 0;
}
