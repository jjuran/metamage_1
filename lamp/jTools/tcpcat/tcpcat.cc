/*	========
 *	htget.cc
 *	========
 */

// Standard C/C++
#include <cerrno>
#include <cstdlib>

// Standard C++
#include <string>

// POSIX
#include "arpa/inet.h"
#include "fcntl.h"
#include "netdb.h"
#include "netinet/in.h"
#include "sys/socket.h"
#include "unistd.h"

// Nitrogen
#include "Nitrogen/OpenTransportProviders.h"

// POSeven
#include "POSeven/FileDescriptor.hh"

// Kerosene
#if TARGET_RT_MAC_CFM
#include "SystemCalls.hh"
#endif

// Orion
#include "Orion/Main.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace P7 = POSeven;
namespace O = Orion;


static N::InetHost ResolveHostname( const char* hostname )
{
#if TARGET_RT_MAC_CFM
	
	return N::OTInetStringToAddress( InternetServices(),
	                                 (char*) hostname ).addrs[ 0 ];
	
#else
	
	hostent* hosts = gethostbyname( hostname );
	
	if ( !hosts || h_errno )
	{
		Io::Err << "Domain name lookup failed: " << h_errno << "\n";
		O::ThrowExitStatus( 1 );
	}
	
	in_addr addr = *(in_addr*) hosts->h_addr;
	
	return N::InetHost( addr.s_addr );
	
#endif
}

int O::Main( int argc, char const *const argv[] )
{
	if ( argc != 3 )
	{
		const char* usage = "tcpcat: Usage:  tcpcat <host> <port>\n";
		
		(void) write( STDERR_FILENO, usage, sizeof usage - 1 );
		
		return 1;
	}
	
	const char* hostname = argv[1];
	const char* port_str = argv[2];
	
	P7::FileDescriptor sock = P7::FileDescriptor( socket( PF_INET, SOCK_STREAM, INET_TCP ) );
	
	N::InetPort port = N::InetPort( std::atoi( port_str ) );
	
	N::InetHost ip = ResolveHostname( hostname );
	
	struct sockaddr_in inetAddress = { 0 };
	
	inetAddress.sin_family = AF_INET;
	inetAddress.sin_port = htons( port );
	inetAddress.sin_addr.s_addr = ip;
	
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

