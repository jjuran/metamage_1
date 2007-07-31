/*	========
 *	htget.cc
 *	========
 */

// Standard C++
#include <string>

// Standard C/C++
#include <cerrno>
#include <cstdlib>

// POSIX
#include "arpa/inet.h"
#include "fcntl.h"
#include "netdb.h"
#include "netinet/in.h"
#include "sys/socket.h"
#include "unistd.h"

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

int O::Main( int argc, char const *const argv[] )
{
	if ( argc != 3 )
	{
		const char usage[] = "tcpcat: Usage:  tcpcat <host> <port>\n";
		
		(void) write( STDERR_FILENO, usage, sizeof usage - 1 );
		
		return 1;
	}
	
	const char* hostname = argv[1];
	const char* port_str = argv[2];
	
	P7::FileDescriptor sock = P7::FileDescriptor( socket( PF_INET, SOCK_STREAM, IPPROTO_TCP ) );
	
	short port = std::atoi( port_str );
	
	struct in_addr ip = ResolveHostname( hostname );
	
	struct sockaddr_in inetAddress = { 0 };
	
	inetAddress.sin_family = AF_INET;
	inetAddress.sin_port   = htons( port );
	inetAddress.sin_addr   = ip;
	
	P7::ThrowPOSIXResult( connect( sock, (const sockaddr*) &inetAddress, sizeof (sockaddr_in) ) );
	
	while ( true )
	{
		enum { blockSize = 4096 };
		char data[ blockSize ];
		
		std::size_t bytesToRead = blockSize;
		
		int received = read( sock, data, bytesToRead );
		
		if ( received == 0 )
		{
			break;
		}
		else if ( received == -1 )
		{
			std::perror( "tcpcat" );
			
			return 1;
		}
		
		(void) io::write( P7::kStdOut_FileNo, data, received );
	}
	
	return 0;
}

