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
		const std::size_t data_size = 4096;
		
		char buffer[ data_size ];
		
		try
		{
			while ( int bytes_read = p7::read( message_body, buffer, data_size ) )
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
			
			p = colon;
			
			while ( *++p == ' ' )  continue;
			
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
		
		itsPartialContent.append( data, byteCount );
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
			
			// Anything left over is content
			std::size_t leftOver = itsReceivedData.size() - startOfContent;
			
			// Start writing content if we have any
			if ( leftOver > 0 )
			{
				ReceiveContent( itsReceivedData.data() + startOfContent, leftOver );
				
				itsReceivedData.resize( startOfContent );
			}
			
			const char* header_stream = GetHeaderStream();
			
			itsHeaderIndex = MakeHeaderIndex( header_stream, &*itsReceivedData.end() );
			
			if ( const HeaderIndexTuple* contentLengthTuple = FindHeaderInStream( header_stream,
			                                                                      itsHeaderIndex,
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
	
	bool MessageReceiver::ReceiveBlock( p7::fd_t socket )
	{
		try
		{
			enum { blockSize = 1024 };
			char data[ blockSize ];
			std::size_t bytesToRead = blockSize;
			
			if ( itHasReceivedAllHeaders && itsContentLengthIsKnown )
			{
				std::size_t bytesToGo = itsContentLength - itsContentBytesReceived;
				
				if ( bytesToGo == 0 )
				{
					return false;
				}
				
				bytesToRead = std::min( bytesToRead, bytesToGo );
			}
			
			int received = p7::read( socket, data, bytesToRead );
			
			if ( received == 0 )
			{
				throw io::end_of_input();
			}
			
			ReceiveData( data, received );
		}
		catch ( const io::end_of_input& )
		{
			itHasReachedEndOfInput = true;
			
			if ( itsContentLengthIsKnown  &&  itsContentBytesReceived != itsContentLength )
			{
				throw IncompleteMessageBody();
			}
			
			return false;
		}
		
		return true;
	}
	
	void MessageReceiver::ReceiveHeaders( p7::fd_t socket )
	{
		while ( !itHasReceivedAllHeaders && ReceiveBlock( socket ) )
		{
			continue;
		}
	}
	
	void MessageReceiver::Receive( p7::fd_t socket )
	{
		while ( ReceiveBlock( socket ) )
		{
			continue;
		}
	}
	
	std::string MessageReceiver::GetHeader( const std::string& name, const char* nullValue )
	{
		const char* stream = GetHeaderStream();
		
		const HeaderIndexTuple* it = FindHeaderInStream( stream,
		                                                 itsHeaderIndex,
		                                                 name.data(),
		                                                 name.size() );
		
		if ( it != NULL )
		{
			return std::string( stream + it->value_offset,
				                stream + it->crlf_offset );
		}
		
		if ( nullValue == NULL )
		{
			throw NoSuchHeader();
		}
		
		return nullValue;
	}
	
	
	std::string ResponseReceiver::GetResult() const
	{
		const std::string& message = GetMessageStream();
		
		if ( const char* p = std::strchr( message.data(), ' ' ) )
		{
			return std::string( p + 1, GetHeaderStream() - 2 );
		}
		
		return "";
	}
	
	unsigned ResponseReceiver::GetResultCode() const
	{
		const std::string& message = GetMessageStream();
		
		if ( const char* p = std::strchr( message.data(), ' ' ) )
		{
			return std::atoi( p + 1 );
		}
		
		return 0;
	}
	
}

