/*	=======
 *	HTTP.cc
 *	=======
 */

#include "HTTP.hh"

// Standard C/C++
#include <cerrno>
#include <cstdlib>
#include <cstring>

// Standard C++
#include <vector>

// POSIX
#include <sys/stat.h>

// Iota
#include "iota/strings.hh"

// Nucleus
#include "Nucleus/Convert.h"
#include "Nucleus/NAssert.h"


namespace NN = Nucleus;
namespace P7 = POSeven;
namespace p7 = poseven;


namespace HTTP
{
	
	// Can be used to construct a message header (for sending)
	struct Header
	{
		std::string name;
		std::string value;
		
		Header()  {}
		
		Header( const std::string& n, const std::string& v ) : name( n ), value( v )  {}
	};
	
	// Used to process an incoming message header
	struct HeaderIndexTuple
	{
		std::size_t header_offset;
		std::size_t colon_offset;
		std::size_t value_offset;
		std::size_t crlf_offset;
	};
	
	typedef std::vector< HeaderIndexTuple > HeaderIndex;
	
	
	class MessageReceiver
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
			void ReceiveData   ( const char* data, std::size_t byteCount );
		
		public:
			MessageReceiver( p7::fd_t header_out,
			                 p7::fd_t body_out ) : itsHeaderOutput              ( header_out ),
			                                       itsBodyOutput                ( body_out   ),
			                                       itsStartOfHeaders            ( 0 ),
			                                       itsPlaceToLookForEndOfHeaders( 0 ),
			                                       itsContentLength             ( 0 ),
			                                       itsContentBytesReceived      ( 0 ),
			                                       itHasReceivedAllHeaders      ( false ),
			                                       itsContentLengthIsKnown      ( false ),
			                                       itHasReachedEndOfInput       ( false )
			{
			}
			
			const std::string& Receive( p7::fd_t socket );
	};
	
	
	static std::string HeaderLine( const Header& header )
	{
		return HeaderLine( header.name, header.value );
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
	
	std::string GetContentLengthHeaderLine( p7::fd_t message_body )
	{
		return HeaderLine( "Content-Length", NN::Convert< std::string >( GetContentLength( message_body ) ) );
	}
	
	static Header GetContentLengthHeader( p7::fd_t message_body )
	{
		return Header( "Content-Length", NN::Convert< std::string >( GetContentLength( message_body ) ) );
	}
	
	void SendMessageBody( p7::fd_t  out,
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
	
	
	void MessageReceiver::ReceiveContent( const char* data, std::size_t byteCount )
	{
		itsContentBytesReceived += byteCount;
		
		io::write( itsBodyOutput, data, byteCount );
	}
	
	void MessageReceiver::ReceiveData( const char* data, std::size_t byteCount )
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
					
					itsPlaceToLookForEndOfHeaders = crlf;
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
			
			// The content starts after the two CRLF
			std::size_t startOfContent = eohMarker + STRLEN( "\r\n" "\r\n" );
			
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
			
			HeaderIndex index = MakeHeaderIndex( header_stream, &*itsReceivedData.end() );
			
			if ( const HeaderIndexTuple* contentLengthTuple = FindHeaderInStream( header_stream,
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
	
	const std::string& MessageReceiver::Receive( p7::fd_t socket )
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
			
			if ( itsContentLengthIsKnown  &&  itsContentBytesReceived != itsContentLength )
			{
				throw IncompleteMessageBody();
			}
		}
		
		return itsReceivedData;
	}
	
	std::string ReceiveMessage( p7::fd_t in,
	                            p7::fd_t header_out,
	                            p7::fd_t body_out )
	{
		MessageReceiver receiver( header_out, body_out );
		
		return receiver.Receive( in );
	}
	
}

