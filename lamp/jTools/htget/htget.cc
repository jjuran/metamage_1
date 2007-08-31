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
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

// Iota
#include "iota/strings.hh"

// Nucleus
#include "Nucleus/Convert.h"
#include "Nucleus/NAssert.h"

// POSeven
#include "POSeven/Open.hh"

// Io
#include "Io/TextInput.hh"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"


namespace NN = Nucleus;
namespace P7 = POSeven;
namespace p7 = poseven;
namespace O = Orion;


namespace HTTP
{
	
	static std::string RequestLine( const std::string& method, const std::string& urlPath )
	{
		return method + " " + urlPath + " HTTP/1.0" "\r\n";
	}
	
	class MalformedHeader {};
	
	struct HeaderIndexTuple
	{
		std::size_t header_offset;
		std::size_t colon_offset;
		std::size_t value_offset;
		std::size_t crlf_offset;
	};
	
	inline HeaderIndexTuple MakeHeaderIndexTuple( std::size_t header,
	                                              std::size_t colon,
	                                              std::size_t value,
	                                              std::size_t crlf )
	{
		HeaderIndexTuple result;
		
		result.header_offset = header;
		result.colon_offset  = colon;
		result.value_offset  = value;
		result.crlf_offset   = crlf;
		
		return result;
	}
	
	typedef std::vector< HeaderIndexTuple > HeaderIndex;
	
	static HeaderIndex MakeHeaderIndex( const char* header_stream, const char* end )
	{
		HeaderIndex result;
		
		const char* p = header_stream;
		
		while ( p < end )
		{
			const char* header = p;
			
			const char* crlf = std::strstr( p, "\r\n" );
			
			if ( crlf == p )
			{
				// Empty line indicates end of headers
				return result;
			}
			
			ASSERT( crlf != NULL );
			
			const char* colon = std::strchr( p, ':' );
			
			if ( colon == NULL  ||  colon > crlf )
			{
				throw MalformedHeader();
			}
			
			while ( *++p != ' ' )  continue;
			
			const char* value = p;
			
			HeaderIndexTuple tuple = MakeHeaderIndexTuple( header - header_stream,
			                                               colon  - header_stream,
			                                               value  - header_stream,
			                                               crlf   - header_stream );
			
			result.push_back( tuple );
			
			p = crlf + STRLEN( "\r\n" );
		}
		
		// Ran out of data without finding end of headers
		throw MalformedHeader();
	}
	
	static bool strings_case_insensitively_equal( const char* a, std::size_t a_len,
	                                              const char* b, std::size_t b_len )
	{
		if ( a_len != b_len )  return false;
		
		for ( const char* a_end = a + a_len;  a < a_end;  ++a, ++b )
		{
			if ( std::tolower( *a ) != std::tolower( *b ) )
			{
				return false;
			}
		}
		
		return true;
	}
	
	static const HeaderIndexTuple* FindHeaderInStream( const char*         stream,
	                                                   const HeaderIndex&  index,
	                                                   const char*         name,
	                                                   std::size_t         name_length )
	{
		for ( HeaderIndex::const_iterator it = index.begin();  it != index.end();  ++it )
		{
			const char* header = stream + it->header_offset;
			
			std::size_t length = it->colon_offset - it->header_offset;
			
			if ( strings_case_insensitively_equal( header, length, name, name_length ) )
			{
				return &*it;
			}
		}
		
		return NULL;
	}
	
	
	struct Header
	{
		std::string name;
		std::string value;
		
		Header()  {}
		
		Header( const std::string& n, const std::string& v ) : name( n ), value( v )  {}
	};
	
	static std::string HeaderLine( const std::string& name, const std::string& value )
	{
		return name + ": " + value + "\r\n";
	}
	
	static std::string HeaderLine( const Header& header )
	{
		return HeaderLine( header.name, header.value );
	}
	
	static void SendMessageHeader( p7::fd_t            out,
	                               const std::string&  message_header )
	{
		p7::write( out, message_header.data(), message_header.size() );
	}
	
	static void SendMessageBody( p7::fd_t  out,
	                             p7::fd_t  message_body )
	{
		char buffer[ 4096 ];
		
		try
		{
			while ( int bytes_read = p7::read( message_body, buffer, 4096 ) )
			{
				p7::write( out, buffer, bytes_read );
			}
		}
		catch ( const io::end_of_input& )
		{
		}
	}
	
	static void SendMessage( p7::fd_t            out,
	                         const std::string&  message_header,
	                         p7::fd_t            message_body )
	{
		SendMessageHeader( out, message_header );
		SendMessageBody  ( out, message_body   );
	}
	
	static std::size_t GetContentLength( p7::fd_t message_body )
	{
		struct stat sb;
		
		P7::ThrowPOSIXResult( fstat( message_body, &sb ) );
		
		if ( !S_ISREG( sb.st_mode ) )
		{
			P7::ThrowErrno( ESPIPE );
		}
		
		return sb.st_size;
	}
	
	static Header GetContentLengthHeader( p7::fd_t message_body )
	{
		return Header( "Content-Length", NN::Convert< std::string >( GetContentLength( message_body ) ) );
	}
	
}

namespace poseven
{
	
	static void connect( fd_t sock, const sockaddr_in& serverAddr )
	{
		P7::ThrowPOSIXResult( ::connect( sock, (const sockaddr*) &serverAddr, sizeof (sockaddr_in) ) );
	}
	
}

namespace htget
{
	
	class HTTPClientTransaction
	{
		private:
			p7::fd_t itsHeaderOutput;
			p7::fd_t itsBodyOutput;
			std::string itsReceivedData;
			std::size_t itsStartOfHeaders;
			std::size_t itsPlaceToLookForEndOfHeaders;
			std::size_t itsContentLength;
			std::size_t itsContentBytesReceived;
			bool itHasReceivedAllHeaders;
			bool itsContentLengthIsKnown;
			bool itHasReachedEndOfInput;
			
			void ReceiveContent( const char* data, std::size_t byteCount );
			void ReceiveData( const char* data, std::size_t byteCount );
			bool IsComplete();
		
		public:
			HTTPClientTransaction( p7::fd_t header_out, p7::fd_t body_out )
			:
				itsHeaderOutput        ( header_out ),
				itsBodyOutput          ( body_out   ),
				itsStartOfHeaders      ( 0 ),
				itsPlaceToLookForEndOfHeaders(),
				itsContentLength       ( 0 ),
				itsContentBytesReceived( 0 ),
				itHasReceivedAllHeaders( false ),
				itsContentLengthIsKnown( false ),
				itHasReachedEndOfInput ( false )
			{
			}
			
			void Download( p7::fd_t socket );
	};
	
	
	bool HTTPClientTransaction::IsComplete()
	{
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
	
	void HTTPClientTransaction::Download( p7::fd_t socket )
	{
		try
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
				
				int received = p7::read( socket, data, bytesToRead );
				
				ReceiveData( data, received );
			}
		}
		catch ( const io::end_of_input& )
		{
			itHasReachedEndOfInput = true;
		}
		
		(void) IsComplete();
	}
	
	void HTTPClientTransaction::ReceiveContent( const char* data, std::size_t byteCount )
	{
		itsContentBytesReceived += byteCount;
		
		io::write( itsBodyOutput, data, byteCount );
	}
	
	void HTTPClientTransaction::ReceiveData( const char* data, std::size_t byteCount )
	{
		// Are we receiving headers or content?
		if ( !itHasReceivedAllHeaders )
		{
			// We haven't received all the headers yet.
			// We need to concatenate all received data to check for the EOH marker.
			itsReceivedData.append( data, byteCount );
			
			if ( itsStartOfHeaders == 0 )
			{
				if ( std::size_t crlf = itsReceivedData.find( "\r\n" ) )
				{
					itsStartOfHeaders = crlf + STRLEN( "\r\n" );
					
					itsPlaceToLookForEndOfHeaders = itsStartOfHeaders;
				}
			}
			
			if ( itsPlaceToLookForEndOfHeaders == 0 )
			{
				return;
			}
			
			if ( itsReceivedData.size() - itsPlaceToLookForEndOfHeaders < STRLEN( "\r\n" "\r\n" ) )
			{
				return;
			}
			
			std::size_t eohMarker = itsReceivedData.find( "\r\n" "\r\n", itsPlaceToLookForEndOfHeaders );
			
			if ( eohMarker == itsReceivedData.npos )
			{
				itsPlaceToLookForEndOfHeaders = itsReceivedData.size() - STRLEN( "\r\n" "\r\n" ) + 1;
				
				return;
			}
			
			// Found it, so we've got all the headers now
			itHasReceivedAllHeaders = true;
			
			// The first CRLF ends the headers
			std::size_t endOfHeaders = eohMarker + 2;
			
			// The content starts after the second
			std::size_t startOfContent = endOfHeaders + 2;
			
			if ( itsHeaderOutput != -1 )
			{
				io::write( itsHeaderOutput, itsReceivedData.data(), startOfContent );
			}
			
			if ( itsBodyOutput == -1 )
			{
				// -1 means we're not expecting a message body, e.g. HEAD
				// itsContentLength and itsContentBytesReceived are already 0
				itsContentLengthIsKnown = true;
				
				return;
			}
			
			// Anything left over is content
			std::size_t leftOver = itsReceivedData.size() - startOfContent;
			
			// Start writing content if we have any
			if ( leftOver > 0 )
			{
				ReceiveContent( itsReceivedData.data() + startOfContent, leftOver );
				
				itsReceivedData.resize( startOfContent );
			}
			
			const char* header_stream = itsReceivedData.data() + itsStartOfHeaders;
			
			HTTP::HeaderIndex index = HTTP::MakeHeaderIndex( header_stream, &*itsReceivedData.end() );
			
			if ( const HTTP::HeaderIndexTuple* contentLengthTuple = HTTP::FindHeaderInStream( header_stream,
			                                                                                  index,
			                                                                                  STR_LEN( "Content-Length" ) ) )
			{
				std::string contentLength( header_stream + contentLengthTuple->value_offset,
				                           header_stream + contentLengthTuple->crlf_offset );
				
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
		else
		{
			// We already have the headers, just count and write the data
			ReceiveContent( data, byteCount );
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
		Io::Err << "Domain name lookup failed: " << h_errno << "\n";
		O::ThrowExitStatus( 1 );
	}
	
	in_addr addr = *(in_addr*) hosts->h_addr;
	
	return addr;
}

int O::Main( int argc, argv_t argv )
{
	bool sendHEADRequest = false;
	bool dumpHeaders     = false;
	bool saveToFile      = false;
	
	const char* defaultOutput = "/dev/fd/1";
	
	const char* outputFile = defaultOutput;
	
	O::BindOption( "-i", dumpHeaders     );
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
		Io::Err << "htget: Usage:  htget <url>\n";
		return 1;
	}
	
	bool expectNoContent = false;
	
	std::string method = "GET";
	
	if ( sendHEADRequest )
	{
		dumpHeaders = true;
		expectNoContent = true;
		method = "HEAD";
	}
	
	if ( expectNoContent  &&  (saveToFile  ||  outputFile != defaultOutput) )
	{
		Io::Err << "htget: Can't save null document to file\n";
		return 1;
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
	
	p7::connect( sock, inetAddress );
	
	std::string message_header =   HTTP::RequestLine( method, urlPath )
	                             + HTTP::HeaderLine( "Host", hostname )
	                             + "\r\n";
	
	HTTP::SendMessageHeader( sock, message_header );
	
	shutdown( sock, SHUT_WR );
	
	const p7::fd_t nil_fd = p7::fd_t( -1 );
	
	p7::fd_t headerOutput = dumpHeaders ? p7::stdout_fileno : nil_fd;
	
	int create_flags = outputIsToFile ? O_CREAT | O_EXCL : 0;
	
	NN::Owned< p7::fd_t > bodyOutput = p7::open( outputFile, O_WRONLY | create_flags, 0644 );
	
	htget::HTTPClientTransaction transaction( headerOutput, expectNoContent ? nil_fd : bodyOutput.Get() );
	
	transaction.Download( sock );
	
	shutdown( sock, SHUT_RD );
	
	return 0;
}

