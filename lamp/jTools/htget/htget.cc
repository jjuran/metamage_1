/*	========
 *	htget.cc
 *	========
 */

// Standard C/C++
#include <cerrno>
#include <cstdlib>

// POSIX
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

// Iota
#include "iota/strings.hh"

// Nucleus
#include "Nucleus/Convert.h"
#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/Open.hh"

// Arcana
#include "HTTP.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"


namespace NN = Nucleus;
namespace p7 = poseven;
namespace O = Orion;


namespace poseven
{
	
	static void connect( fd_t sock, const sockaddr_in& serverAddr )
	{
		p7::throw_posix_result( ::connect( sock, (const sockaddr*) &serverAddr, sizeof (sockaddr_in) ) );
	}
	
}

namespace htget
{
	
	static bool ParseURL( const std::string& url,
	                      std::string& outURLScheme, 
	                      std::string& outHostname,
	                      std::string& outPort,
	                      std::string& outURLpath )
	{
		std::size_t colonSlashSlash = url.find( "://" );
		
		if ( colonSlashSlash == url.npos )
		{
			return false;
		}
		
		outURLScheme = url.substr( 0, colonSlashSlash );
		
		std::size_t hostnameStart = colonSlashSlash + 3;
		std::size_t slash = url.find( "/", hostnameStart );
		std::size_t colon = url.find( ":", hostnameStart );
		std::size_t hostnameEnd = std::min( slash, colon );
		
		outHostname = url.substr( hostnameStart, hostnameEnd - hostnameStart );
		
		if ( colon < slash )
		{
			outPort = url.substr( colon + 1, slash - (colon + 1) );
		}
		
		outURLpath = (slash == url.npos) ? std::string( "/" ) : url.substr( slash, url.npos );
		
		return true;
	}
	
}


static const char* DocName( const std::string& urlPath )
{
	std::size_t lastSlash = urlPath.find_last_of( "/" );
	
	// Skip the slash.
	// If there wasn't one, then lastSlash == string::npos == 0xFFFFFFFF == -1.
	// Adding one then yields zero, which is exactly what we want.
	
	return urlPath.c_str() + lastSlash + 1;
}


static struct in_addr ResolveHostname( const char* hostname )
{
	hostent* hosts = gethostbyname( hostname );
	
	if ( !hosts || h_errno )
	{
		std::string message = "Domain name lookup failed: " + NN::Convert< std::string >( h_errno ) + "\n";
		
		p7::write( p7::stderr_fileno, message.data(), message.size() );
		
		O::ThrowExitStatus( 1 );
	}
	
	in_addr addr = *(in_addr*) hosts->h_addr;
	
	return addr;
}

int O::Main( int argc, argv_t argv )
{
	bool sendHEADRequest = false;
	bool dumpHeader      = false;
	bool saveToFile      = false;
	
	const char* defaultOutput = "/dev/fd/1";
	
	const char* outputFile = defaultOutput;
	
	O::BindOption( "-i", dumpHeader      );
	O::BindOption( "-I", sendHEADRequest );
	O::BindOption( "-o", outputFile      );
	O::BindOption( "-O", saveToFile      );
	
	O::AliasOption( "-i", "--headers" );
	O::AliasOption( "-i", "--include" );
	
	O::AliasOption( "-I", "--head" );
	
	O::AliasOption( "-O", "--remote-name" );
	O::AliasOption( "-O", "--save"        );
	
	O::GetOptions( argc, argv );
	
	char const *const *freeArgs = O::FreeArguments();
	
	std::size_t argCount = O::FreeArgumentCount();
	
	if ( argCount == 0 )
	{
		p7::write( p7::stderr_fileno, STR_LEN( "htget: Usage:  htget <url>\n" ) );
		
		return EXIT_FAILURE;
	}
	
	bool expectNoContent = false;
	
	std::string method = "GET";
	
	if ( sendHEADRequest )
	{
		dumpHeader = true;
		expectNoContent = true;
		method = "HEAD";
	}
	
	if ( expectNoContent  &&  (saveToFile  ||  outputFile != defaultOutput) )
	{
		p7::write( p7::stderr_fileno, STR_LEN( "htget: Can't save null document to file\n" ) );
		
		return EXIT_FAILURE;
	}
	
	std::string scheme;
	std::string hostname;
	std::string urlPath;
	std::string portStr;
	
	short defaultPort = 0;
	
	bool parsed = htget::ParseURL( freeArgs[ 0 ], scheme, hostname, portStr, urlPath );
	
	// FIXME:  Eliminate . and .. from urlPath
	
	if ( saveToFile )
	{
		outputFile = DocName( urlPath );
	}
	
	bool outputIsToFile = outputFile != defaultOutput;
	
	p7::fd_t sock = p7::fd_t( socket( PF_INET, SOCK_STREAM, IPPROTO_TCP ) );
	
	if ( scheme == "http" )
	{
		defaultPort = 80;
	}
	else
	{
		std::string message = "Unsupported scheme '" + scheme + "'.\n";
		
		p7::write( p7::stderr_fileno, message.data(), message.size() );
		
		return 2;
	}
	
	short port = ( !portStr.empty() ) ? std::atoi( portStr.c_str() )
	                                  : defaultPort;
	
	struct in_addr ip = ResolveHostname( hostname.c_str() );
	
	struct sockaddr_in inetAddress = { 0 };
	
	inetAddress.sin_family = AF_INET;
	inetAddress.sin_port   = htons( port );
	inetAddress.sin_addr   = ip;
	
	p7::connect( sock, inetAddress );
	
	std::string message_header =   HTTP::RequestLine( method, urlPath )
	                             + HTTP::HeaderFieldLine( "Host", hostname )
	                             + "\r\n";
	
	HTTP::SendMessageHeader( sock, message_header );
	
	p7::oflag_t create_flags = outputIsToFile ? p7::o_creat | p7::o_excl : p7::oflag_t();
	
	NN::Owned< p7::fd_t > bodyOutput = p7::open( outputFile, p7::o_wronly | create_flags, 0644 );
	
	HTTP::ResponseReceiver response;
	
	response.ReceiveHeader( sock );
	
	if ( dumpHeader )
	{
		const std::string& message = response.GetMessageStream();
		
		p7::write( p7::stdout_fileno, message.data(), message.size() );
	}
	
	if ( !expectNoContent )
	{
		const std::string& partial_content = response.GetPartialContent();
		
		if ( !partial_content.empty() )
		{
			p7::write( bodyOutput, partial_content.data(), partial_content.size() );
		}
		
		HTTP::SendMessageBody( bodyOutput, sock );
	}
	
	return 0;
}

