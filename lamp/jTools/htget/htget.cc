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
#include "Nitrogen/Files.h"

// POSeven
#include "POSeven/FileDescriptor.hh"

// Io
#include "Io/TextInput.hh"

// BitsAndBytes
#include "HexStrings.hh"

// Kerosene
#if TARGET_RT_MAC_CFM
#include "SystemCalls.hh"
#endif

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace P7 = POSeven;
namespace O = Orion;
namespace Bits = BitsAndBytes;


namespace htget
{
	
	class HTTPClientTransaction
	{
		private:
			P7::FileDescriptor itsReceiver;
			P7::FileDescriptor itsSocket;
			std::string itsReceivedData;
			std::size_t itsContentLength;
			std::size_t itsContentBytesReceived;
			bool itHasReceivedAllHeaders;
			bool itsContentLengthIsKnown;
			bool itHasReachedEndOfInput;
			
			void ReceiveData( const char* data, std::size_t byteCount );
			bool IsComplete();
		
		public:
			HTTPClientTransaction( P7::FileDescriptor  sock,
			                       const sockaddr_in&  serverAddr )
			:
				itsReceiver            ( P7::kStdOut_FileNo ),
				itsSocket              ( sock ),
				itsContentLength       ( 0 ),
				itsContentBytesReceived( 0 ),
				itHasReceivedAllHeaders( false ),
				itsContentLengthIsKnown( false ),
				itHasReachedEndOfInput ( false )
			{
				P7::ThrowPOSIXResult( connect( sock, (const sockaddr*) &serverAddr, sizeof (sockaddr_in) ) );
			}
			
			~HTTPClientTransaction()  {}
			
			void Download();
	};
	
}

namespace Nucleus
{
	
	template <>
	struct OwnedDefaults< htget::HTTPClientTransaction* >
	{
		typedef DisposeWithDelete Disposer;
	};
	
}

namespace htget
{
	
	namespace Globals
	{
		
		bool gDumpHeaders     = false;
		bool gExpectNoContent = false;
		bool gSaveToFile      = false;
		
		std::string gSaveLocation;
		
		NN::Owned< HTTPClientTransaction* > gTransaction;
		
	}
	
	using namespace Globals;
	
	
	bool HTTPClientTransaction::IsComplete()
	{
		if ( itsContentLengthIsKnown  &&  itsContentBytesReceived > itsContentLength )
		{
			Io::Err << "Error:  Received more data (" 
			        << itsContentBytesReceived
			        << " bytes) than was specified ("
			        << itsContentLength
			        << " bytes).\n";
			
			return true;
		}
		
		if ( itHasReachedEndOfInput && itsContentLengthIsKnown )
		{
			Io::Err << "Error: remote socket closed, "
			        << itsContentBytesReceived
			        << " out of "
			        << itsContentLength
			        << " bytes received.\n";
		}
		
		if ( itHasReachedEndOfInput )  return true;
		
		if ( itHasReceivedAllHeaders && itsContentLengthIsKnown )
		{
			return ( itsContentBytesReceived == itsContentLength );
		}
		
		return false;
	}
	
	void HTTPClientTransaction::Download()
	{
		while ( true )
		{
			enum { blockSize = 1024 };
			char data[ blockSize ];
			std::size_t bytesToRead = blockSize;
			
			if ( itHasReceivedAllHeaders && itsContentLengthIsKnown )
			{
				std::size_t bytesToGo = itsContentLength - itsContentBytesReceived;
				
				if ( bytesToGo == 0 )  break;
				
				bytesToRead = std::min( bytesToRead, bytesToGo );
			}
			
			int received = read( itsSocket, data, bytesToRead );
			
			if ( received == 0 )
			{
				itHasReachedEndOfInput = true;
				break;
			}
			else if ( received == -1 )
			{
				Io::Err << "Errno " << errno << " on read()\n";
				
				O::ThrowExitStatus( 1 );
			}
			
			ReceiveData( data, received );
		}
		
		(void) IsComplete();
	}
	
	struct TypeAndCreator
	{
		N::OSType type;
		N::OSType creator;
		
		TypeAndCreator( N::OSType type, N::OSType creator )
		:
			type   ( type    ),
			creator( creator )
		{}
	};
	
	static std::string GetHTTPHeader( const std::string&  headerName,
	                                  const std::string&  allHeaders )
	{
		typedef std::size_t Position;
		
		// FIXME:  Be case-insensitive.
		
		// Find the header in the headers
		Position headerPos = allHeaders.find( headerName );
		
		if ( headerPos == allHeaders.npos )
		{
			return "";
		}
		
		// This is never npos if allHeaders is valid.
		Position eol = allHeaders.find( "\r\n", headerPos );
		
		ASSERT( eol != allHeaders.npos );
		
		return allHeaders.substr( headerPos, eol - headerPos );
	}
	
	static std::string GetHTTPHeaderValue( const std::string& headerName, const std::string& allHeaders )
	{
		typedef std::size_t Position;
		
		std::string header = GetHTTPHeader( headerName, allHeaders );
		
		Position valuePos = header.find_first_not_of( " \t", headerName.size() + 1 );
		
		if ( valuePos == header.npos )
		{
			return "";
		}
		
		return header.substr( valuePos, header.npos );
	}
	
	static N::OSType MakeOSType( const std::string& hexCodes )
	{
		return NN::Convert< N::OSType >( Bits::DecodeHex( hexCodes ) );
	}
	
	static NN::Owned< P7::FileDescriptor > CreateAndOpenFileWithSignature( const std::string& pathname, const TypeAndCreator& sig )
	{
		int open_flags = O_WRONLY;
		
	#if TARGET_RT_MAC_CFM
		
		N::FSpCreate( Path2FSS( pathname ),
		              sig.creator,
		              sig.type );
		
	#else
		
		open_flags |= O_CREAT | O_EXCL;
		
	#endif
		
		return P7::Open( pathname.c_str(), open_flags, 0644 );
	}
	
	void HTTPClientTransaction::ReceiveData( const char* data, std::size_t byteCount )
	{
		// Are we receiving headers or content?
		if ( !itHasReceivedAllHeaders )
		{
			// We haven't received all the headers yet.
			// We need to concatenate all received data to check for the EOH marker.
			itsReceivedData += std::string( data, byteCount );
			std::size_t eohMarker = itsReceivedData.find( "\r\n\r\n" );
			
			if ( eohMarker != itsReceivedData.npos )
			{
				// Found it, so we've got all the headers now
				itHasReceivedAllHeaders = true;
				
				// The first CRLF ends the headers
				std::size_t endOfHeaders = eohMarker + 2;
				
				// The content starts after the second
				std::size_t startOfContent = endOfHeaders + 2;
				
				std::string allHeaders = itsReceivedData.substr( 0, startOfContent );
				
				if ( gDumpHeaders )
				{
					io::write( itsReceiver, allHeaders.data(), allHeaders.size() );
				}
				
				if ( gExpectNoContent )
				{
					// itsContentLength and itsContentBytesReceived are already 0
					itsContentLengthIsKnown = true;
					return;
				}
				
				if ( gSaveToFile )
				{
					TypeAndCreator signature( N::kUnknownType, N::kUnknownType );
					
					try
					{
						signature.type    = MakeOSType( GetHTTPHeaderValue( "X-Mac-Type",
						                                                    allHeaders ) );
						
						signature.creator = MakeOSType( GetHTTPHeaderValue( "X-Mac-Creator",
						                                                    allHeaders ) );
					}
					catch ( ... ) {}
					
					itsReceiver = CreateAndOpenFileWithSignature( gSaveLocation, signature ).Release();
				}
				
				// Anything left over is content
				std::size_t leftOver = byteCount - startOfContent;
				itsContentBytesReceived = leftOver;
				
				// Start writing content if we have any
				if ( leftOver > 0 )
				{
					io::write( itsReceiver, itsReceivedData.data() + startOfContent, leftOver );
				}
				
				std::string contentLength = GetHTTPHeaderValue( "Content-Length", allHeaders );
				
				if ( !contentLength.empty() )
				{
					// Now get the *real* value, as opposed to its textual representation
					itsContentLength = std::atoi( contentLength.c_str() );
					itsContentLengthIsKnown = true;
				}
				else
				{
					// itsContentLength is already set to 0
					// itsContentLengthIsKnown is already false
				}
			}
		}
		else
		{
			// We already have the headers, just count and write the data
			itsContentBytesReceived += byteCount;
			
			io::write( itsReceiver, data, byteCount );
		}
	}
	
	
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
	
	
	static NN::Owned< HTTPClientTransaction* >
	NewTransaction( P7::FileDescriptor  socket,
	                const sockaddr_in&  addr )
	{
		return NN::Owned< HTTPClientTransaction* >::Seize
		(
			new HTTPClientTransaction( socket, addr )
		);
	}
	
}


static std::string DocName( const std::string& urlPath )
{
	std::size_t lastSlash = urlPath.find_last_of( "/" );
	
	// Skip the slash.
	// If there wasn't one, then lastSlash == string::npos == 0xFFFFFFFF == -1.
	// Adding one then yields zero, which is exactly what we want.
	
	return urlPath.substr( lastSlash + 1, urlPath.npos );
}


using namespace htget::Globals;

enum
{
	optDumpHeaders, 
	optSendHEADRequest,
	optSaveToFile
};

static O::Options DefineOptions()
{
	O::Options options;
	
	options.DefineSetFlag( "--headers", optDumpHeaders     );
	options.DefineSetFlag( "--head",    optSendHEADRequest );
	options.DefineSetFlag( "--save",    optSaveToFile      );
	
	return options;
}

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

static void WriteLine( P7::FileDescriptor stream, const std::string& text )
{
	std::string line = text + "\r\n";
	
	io::write( stream, line.data(), line.size() );
}

int O::Main( int argc, char const *const argv[] )
{
	Options options = DefineOptions();
	options.GetOptions(argc, argv);
	
	const std::vector< const char* >& params = options.GetFreeParams();
	
	if ( params.empty() )
	{
		Io::Err << "htget: Usage:  htget <url>\n";
		return 1;
	}
	
	if ( options.GetFlag( optDumpHeaders ) )
	{
		gDumpHeaders = true;
	}
	
	std::string method = "GET";
	
	if ( options.GetFlag( optSendHEADRequest ) )
	{
		gDumpHeaders = true;
		gExpectNoContent = true;
		method = "HEAD";
	}
	
	if ( options.GetFlag( optSaveToFile ) )
	{
		if ( gExpectNoContent )
		{
			Io::Err << "htget: Can't save null document to file\n";
			return 1;
		}
		
		gSaveToFile = true;
	}
	
	std::string scheme;
	std::string hostname;
	std::string urlPath;
	std::string portStr;
	
	short defaultPort = 0;
	
	bool parsed = htget::ParseURL( params[ 0 ], scheme, hostname, portStr, urlPath );
	
	// FIXME:  Eliminate . and .. from urlPath
	
	if ( gSaveToFile )
	{
		gSaveLocation = DocName( urlPath );
	}
	
	P7::FileDescriptor sock = P7::FileDescriptor( socket( PF_INET, SOCK_STREAM, INET_TCP ) );
	
	if ( scheme == "http" )
	{
		defaultPort = 80;
	}
	/*
	else if ( scheme == "https" )
	{
		sock = HW::MakeHandleFromCopy< Lox::SecureSocket_Details >( Lox::Socket::New() );
		defaultPort = 443;
	}
	*/
	else
	{
		Io::Err << "Unsupported scheme '" << scheme << "'.\n";
		return 2;
	}
	
	short port = ( !portStr.empty() ) ? std::atoi( portStr.c_str() )
	                                  : defaultPort;
	
	struct in_addr ip = ResolveHostname( hostname.c_str() );
	
	struct sockaddr_in inetAddress = { 0 };
	
	inetAddress.sin_family = AF_INET;
	inetAddress.sin_port   = htons( port );
	inetAddress.sin_addr   = ip;
	
	if ( params.size() > 1 )
	{
		const char* pathname = params[ 1 ];
		
		gSaveLocation = pathname;
	}
	
	gTransaction = htget::NewTransaction( sock, inetAddress );
	
	WriteLine( sock, method + " " + urlPath + " HTTP/1.0" );
	WriteLine( sock, "Host: " + hostname );
	WriteLine( sock, "" );
	
	gTransaction.Get()->Download();
	
	return 0;
}

