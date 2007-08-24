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
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

// POSeven
#include "POSeven/FileDescriptor.hh"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace P7 = POSeven;
namespace O = Orion;


static struct in_addr ResolveHostname( const char* hostname )
{
	hostent* hosts = gethostbyname( hostname );
	
	if ( !hosts || h_errno )
	{
		Io::Err << "Domain name lookup failed: " << h_errno << "\n";
		O::ThrowExitStatus( 1 );
	}
	
	in_addr addr = *(in_addr*) hosts->h_addr;
	
	return addr;
}

static P7::FileDescriptor Connect( const char* hostname, const char* port_str )
{
	P7::FileDescriptor result = P7::FileDescriptor( socket( PF_INET, SOCK_STREAM, IPPROTO_TCP ) );
	
	short port = std::atoi( port_str );
	
	struct in_addr ip = ResolveHostname( hostname );
	
	struct sockaddr_in inetAddress = { 0 };
	
	inetAddress.sin_family = AF_INET;
	inetAddress.sin_port   = htons( port );
	inetAddress.sin_addr   = ip;
	
	P7::ThrowPOSIXResult( connect( result, (const sockaddr*) &inetAddress, sizeof (sockaddr_in) ) );
	
	return result;
}

int O::Main( int argc, char const *const argv[] )
{
	if ( argc < 4 )
	{
		const char usage[] = "tcpclient: Usage:  tcpclient <host> <port> <program argv>\n";
		
		(void) write( STDERR_FILENO, usage, sizeof usage - 1 );
		
		return 1;
	}
	
	const char* hostname = argv[1];
	const char* port_str = argv[2];
	
	char *const *program_argv = (char**) argv + 3;
	
	P7::FileDescriptor sock = Connect( hostname, port_str );
	
	dup2( sock, 6 );
	dup2( sock, 7 );
	
	execvp( program_argv[0], program_argv );
	
	_exit( 127 );
	
	return 0;
}

