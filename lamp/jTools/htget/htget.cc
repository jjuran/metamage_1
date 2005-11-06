/*	========
 *	htget.cc
 *	========
 */

// Standard C/C++
#include <cerrno>
//#include <cstdio>
#include <cstdlib>

// Standard C++
#include <string>

// POSIX
#include "netinet/in.h"
#include "sys/socket.h"
#include "unistd.h"

// Nitrogen
#include "Nitrogen/OpenTransportProviders.h"
#include "Nitrogen/Threads.h"

// Nitrogen Extras / POSeven
#include "POSeven/FileDescriptor.h"

// Io
#include "Io/Exceptions.hh"
#include "Io/Files.hh"
#include "Io/MakeHandle.hh"
#include "Io/Stream.hh"
#include "Io/TextInput.hh"

// BitsAndBytes
#include "HexStrings.hh"

// Locksmith
//#include "SecureSocket.hh"

// Kerosene
#include "SystemCalls.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace N = Nitrogen;
namespace P7 = POSeven;
namespace O = Orion;
//namespace Lox = Locksmith;
namespace Bits = BitsAndBytes;


namespace Io
{
	
	static int Write( P7::FileDescriptor fd, const char* data, std::size_t byteCount )
	{
		int written = write( fd, data, byteCount );
		P7::ThrowPOSIXResult( written );
		
		return written;
	}
	
}

namespace htget
{
	
	static Io::Handle gStdOut = Io::MakeHandleFromCast< Io::FD_Details, Io::FD >( Io::out );
	
	class HTTPClientTransaction
	{
		private:
			Io::Stream< Io::Handle > myReceiver;
			P7::FileDescriptor mySocket;
			Io::StringPipe myPendingWrites;
			std::string myReceivedData;
			int myContentLength;
			int myContentBytesReceived;
			bool myReceivedAllHeadersFlag;
			bool myContentLengthKnown;
			bool myNullReadOccurred;
		
		public:
			HTTPClientTransaction( P7::FileDescriptor  sock,
			                       const sockaddr_in&  serverAddr )
			:
				myReceiver              ( gStdOut ),
				mySocket                ( sock ),
				myContentLength         ( 0 ),
				myContentBytesReceived  ( 0 ),
				myReceivedAllHeadersFlag( false ),
				myContentLengthKnown    ( false ),
				myNullReadOccurred      ( false )
			{
				// Don't make the connect non-blocking until we can handle it
				//mySocket.Connect( serverAddr );
				int result = connect( sock, (const sockaddr*)&serverAddr, sizeof (sockaddr_in) );
				//Socket().SetNonblocking();
			}
			
			~HTTPClientTransaction()  {}
			
			void WriteLine( const std::string& data )
			{
				myPendingWrites.Write( data + "\r\n" );
			}
			
			void WriteLine( const char* data, unsigned int byteCount)
			{
				WriteLine( std::string( data, byteCount ) );
			}
			
			void WriteLine(const char* data)
			{
				WriteLine( std::string( data ) );
			}
			
			void DoIO();
			void ReceiveData( const char* data, unsigned int byteCount );
			bool IsComplete();
	};
	
}

namespace Nitrogen
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
		
		FSSpec gSaveLocation;
		
		N::Owned< HTTPClientTransaction* > gTransaction;
		
	}
	
	using namespace Globals;
	
	
	bool HTTPClientTransaction::IsComplete()
	{
		if ( myContentLengthKnown  &&  myContentBytesReceived > myContentLength )
		{
			Io::Err << "Error:  Received more data (" 
			        << myContentBytesReceived
			        << " bytes) than was specified ("
			        << myContentLength
			        << " bytes).\n";
			
			return true;
		}
		
		if ( myNullReadOccurred && myContentLengthKnown )
		{
			Io::Err << "Error: remote socket closed, "
			        << myContentLengthKnown
			        << " out of "
			        << myContentLength
			        << " bytes received.\n";
		}
		
		if ( myNullReadOccurred )  return true;
		
		if ( myReceivedAllHeadersFlag && myContentLengthKnown )
		{
			return ( myContentBytesReceived == myContentLength );
		}
		
		return false;
	}
	
	void HTTPClientTransaction::DoIO()
	{
		if ( IsComplete() )
		{
			O::ThrowExitStatus( 0 );
		}
		
		while ( myPendingWrites.Ready() )
		{
			std::string str = myPendingWrites.Read();
			Io::S( mySocket ) << str;
		}
		
		//mySocket.SetNonblocking();
		
		while ( true )
		{
			enum { blockSize = 1024 };
			char data[ blockSize ];
			int bytesToRead = blockSize;
			
			if ( myReceivedAllHeadersFlag && myContentLengthKnown )
			{
				int bytesToGo = myContentLength - myContentBytesReceived;
				
				if ( bytesToGo == 0 )  break;
				
				bytesToRead = std::min( bytesToRead, bytesToGo );
			}
			
			try
			{
				//int received = mySocket.Read( data, bytesToRead );
				int received = read( mySocket, data, bytesToRead );
				
				if ( received == 0 )
				{
					myNullReadOccurred = true;
					break;
				}
				else if ( received == -1 )
				{
					Io::Err << "Errno " << errno << " on read()\n";
					
					O::ThrowExitStatus( 1 );
				}
				
				ReceiveData( data, received );
			}
			/*
			catch ( Io::NoDataPending )
			{
				break;
			}
			catch ( HW::PeerClosedSocket )
			{
				myNullReadOccurred = true;
				break;
			}
			catch ( G::POSIXError& error )
			{
				Io::Err << "Read error: " << error.Errno() << "\n";
				O::Return( error.Errno() );
				break;
			}
			*/
			catch ( ... )
			{
				Io::Err << "Unknown read error\n";
			}
		}
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
		using std::string;
		
		typedef string::size_type Position;
		
		// FIXME:  Be case-insensitive.
		
		// Find the header in the headers
		Position headerPos = allHeaders.find( headerName );
		
		if ( headerPos == string::npos )
		{
			return "";
		}
		
		// This is never npos if allHeaders is valid.
		Position eol = allHeaders.find( "\r\n", headerPos );
		
		return allHeaders.substr( headerPos, eol - headerPos );
	}
	
	static std::string GetHTTPHeaderValue( const std::string& headerName, const std::string& allHeaders )
	{
		using std::string;
		
		typedef string::size_type Position;
		
		string header = GetHTTPHeader( headerName, allHeaders );
		
		Position valuePos = header.find_first_not_of( " \t", headerName.size() + 1 );
		
		return header.substr( valuePos, string::npos );
	}
	
	static N::OSType MakeOSType( const std::string& hexCodes )
	{
		return N::Convert< N::OSType >( Bits::DecodeHex( hexCodes ) );
	}
	
	void HTTPClientTransaction::ReceiveData( const char* data, unsigned int byteCount )
	{
		using std::string;
		
		typedef string::size_type size_type;
		const size_type npos = string::npos;
		
		// Are we receiving headers or content?
		if ( !myReceivedAllHeadersFlag )
		{
			// We haven't received all the headers yet.
			// We need to concatenate all received data to check for the EOH marker.
			myReceivedData += string( data, byteCount );
			size_type eohMarker = myReceivedData.find( "\r\n\r\n" );
			
			if ( eohMarker != npos )
			{
				// Found it, so we've got all the headers now
				myReceivedAllHeadersFlag = true;
				
				// The first CRLF ends the headers
				size_type endOfHeaders = eohMarker + 2;
				
				// The content starts after the second
				size_type startOfContent = endOfHeaders + 2;
				
				std::string allHeaders = myReceivedData.substr( 0, startOfContent );
				
				if ( gDumpHeaders )
				{
					//Io::Put( myReceiver, myReceivedData.data(), startOfContent );
					myReceiver << allHeaders;
				}
				
				if ( gExpectNoContent )
				{
					// myContentLength and myContentBytesReceived are already 0
					myContentLengthKnown = true;
					return;
				}
				
				if ( gSaveToFile )
				{
					TypeAndCreator signature( kUnknownType, kUnknownType );
					
					try
					{
						signature.type    = MakeOSType( GetHTTPHeaderValue( "X-Mac-Type",
						                                                    allHeaders ) );
						
						signature.creator = MakeOSType( GetHTTPHeaderValue( "X-Mac-Creator",
						                                                    allHeaders ) );
					}
					catch ( ... ) {}
					
					myReceiver = Io::MakeHandleFromCopy< Io::FileRefNum_Details >
					(
						N::FSpOpenDF( N::FSpCreate( gSaveLocation,
						                            signature.creator,
						                            signature.type ),
						              fsWrPerm )
					);
				}
				
				// Anything left over is content
				size_type leftOver = byteCount - startOfContent;
				myContentBytesReceived = leftOver;
				
				// Start writing content if we have any
				if ( leftOver > 0 )
				{
					Io::Put( myReceiver, myReceivedData.data() + startOfContent, leftOver );
				}
				
				/*
				string contentLengthName = "Content-Length:";
				// Find the Content-Length header in the headers
				size_type contentLengthHeaderPos = myReceivedData.find( contentLengthName );
				
				// If we can't find it, assume no content
				if ( contentLengthHeaderPos == npos )
				{
					// myContentLength is already set to 0
					// myContentLengthKnown is already false
					return;
				}
				
				// Skip over the key and locate the value
				size_type contentLengthPos = contentLengthHeaderPos + contentLengthName.size() + 1;
				
				// Get the length of the value (not the most efficient way, but it works)
				size_type contentLengthLen = myReceivedData.substr( contentLengthPos, npos ).find( "\r\n" );
				
				// Get the value
				string contentLength = myReceivedData.substr( contentLengthPos, contentLengthLen );
				*/
				
				try
				{
					std::string contentLength = GetHTTPHeaderValue( "Content-Length", allHeaders );
					
					// Now get the *real* value, as opposed to its textual representation
					myContentLength = std::atoi( contentLength.c_str() );
					myContentLengthKnown = true;
				}
				catch ( ... )
				{
					// myContentLength is already set to 0
					// myContentLengthKnown is already false
				}
			}
		}
		else
		{
			// We already have the headers, just count and write the data
			myContentBytesReceived += byteCount;
			Io::Put( myReceiver, data, byteCount );
		}
	}
	
	
	static bool ParseURL( const std::string& url,
	                      std::string& outURLScheme, 
	                      std::string& outHostname,
	                      std::string& outPort,
	                      std::string& outURLpath )
	{
		using std::string;
		/*
		// Should work but doesn't
		using string::size_type;
		using string::npos;
		*/
		typedef string::size_type size_type;
		const size_type npos = string::npos;
		
		size_type colonSlashSlash = url.find( "://" );
		
		if ( colonSlashSlash == npos )
		{
			return false;
		}
		
		outURLScheme = url.substr( 0, colonSlashSlash );
		
		size_type hostnameStart = colonSlashSlash + 3;
		size_type slash = url.find( "/", hostnameStart );
		size_type colon = url.find( ":", hostnameStart );
		size_type hostnameEnd = std::min( slash, colon );
		
		outHostname = url.substr( hostnameStart, hostnameEnd - hostnameStart );
		
		if ( colon < slash )
		{
			outPort = url.substr( colon + 1, slash - (colon + 1) );
		}
		
		outURLpath = (slash == npos) ? string( "/" ) : url.substr( slash, npos );
		
		return true;
	}
	
	
	static N::Owned< HTTPClientTransaction* >
	NewTransaction( P7::FileDescriptor  socket,
	                const sockaddr_in&  addr )
	{
		return N::Owned< HTTPClientTransaction* >::Seize
		(
			new HTTPClientTransaction( socket, addr )
		);
	}
	
}


static std::string DocName( const std::string& urlPath )
{
	using std::string;
	
	string::size_type lastSlash = urlPath.find_last_of( "/" );
	
	// Skip the slash.
	// If there wasn't one, then lastSlash == string::npos == 0xFFFFFFFF == -1.
	// Adding one then yields zero, which is exactly what we want.
	
	return urlPath.substr( lastSlash + 1, string::npos );
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
	
	N::InetPort defaultPort = 0;
	
	bool parsed = htget::ParseURL( params[ 0 ], scheme, hostname, portStr, urlPath );
	
	// FIXME:  Eliminate . and .. from urlPath
	
	if ( gSaveToFile )
	{
		gSaveLocation = Path2FSS( DocName( urlPath ) );
	}
	
	P7::FileDescriptor sock = socket( PF_INET, SOCK_STREAM, INET_TCP );
	
	if ( scheme == "http" )
	{
		//sock = HW::MakeHandleFromCopy< HW::TCPSocket_Details >( HW::NewSocket() );
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
	
	N::InetPort port = ( !portStr.empty() ) ? std::atoi( portStr.c_str() )
	                                        : defaultPort;
	
	N::InetHost ip = N::OTInetStringToAddress( InternetServices(),
	                                           (char*)hostname.c_str() ).addrs[ 0 ];
	
	struct sockaddr_in inetAddress;
	
	inetAddress.sin_family = AF_INET;
	inetAddress.sin_port = port;
	inetAddress.sin_addr.s_addr = ip;
	
	if ( params.size() > 1 )
	{
		const char* pathname = params[ 1 ];
		
		gSaveLocation = Path2FSS( pathname );
	}
	
	gTransaction = htget::NewTransaction( sock, inetAddress );
	
	gTransaction.Get()->WriteLine( method + " " + urlPath + " HTTP/1.0" );
	gTransaction.Get()->WriteLine( "Host: " + hostname );
	gTransaction.Get()->WriteLine( "" );
	
	while ( !gTransaction.Get()->IsComplete() )
	{
		sleep( 0 );
		gTransaction.Get()->DoIO();
	}
	
	return 0;
}

