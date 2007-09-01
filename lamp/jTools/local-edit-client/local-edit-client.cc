/*	====================
 *	local-edit-client.cc
 *	====================
 */

// Standard C/C++
#include <cerrno>
#include <cstdlib>

// Standard C++
#include <string>

// POSIX
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

// POSeven
#include "POSeven/FileDescriptor.hh"
#include "POSeven/Open.hh"

// Arcana
#include "HTTP.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


namespace NN = Nucleus;
namespace p7 = poseven;
namespace O = Orion;


int O::Main( int argc, argv_t argv )
{
	const char* filename = argc > 1 ? argv[1] : "/dev/null";
	
	NN::Owned< p7::fd_t > message_body = p7::open( filename, O_RDONLY );
	
	const p7::fd_t socket_in  = p7::fd_t( 6 );
	const p7::fd_t socket_out = p7::fd_t( 7 );
	
	const char* method = "APPLY";
	
	const char* urlPath = "/cgi-bin/local-edit-server";
	
	std::string contentLengthHeader;
	
	try
	{
		contentLengthHeader = HTTP::GetContentLengthHeaderLine( message_body );
	}
	catch ( ... )
	{
	}
	
	std::string message_header =   HTTP::RequestLine( method, urlPath )
	                             //+ HTTP::HeaderLine( "Host", hostname )
	                             + contentLengthHeader
	                             + "\r\n";
	
	HTTP::SendMessage( socket_out, message_header, message_body );
	
	shutdown( socket_out, SHUT_WR );
	
	//std::string header_stream = HTTP::ReceiveMessage( socket_in, p7::fd_t( -1 ), p7::stdout_fileno );
	std::string header_stream = HTTP::ReceiveMessage( socket_in, p7::stderr_fileno, p7::stdout_fileno );
	
	shutdown( socket_in, SHUT_RD );
	
	return 0;
}

