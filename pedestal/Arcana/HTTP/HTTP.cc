/*	=======
 *	HTTP.cc
 *	=======
 */

#include "HTTP.hh"

// Standard C/C++
#include <cerrno>
#include <cstring>

// Standard C++
#include <vector>

// iota
#include "iota/char_types.hh"
#include "iota/strings.hh"

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/functions/fstat.hh"
#include "poseven/functions/read.hh"
#include "poseven/types/errno_t.hh"


namespace p7 = poseven;


namespace HTTP
{
	
	static std::size_t GetContentLength( p7::fd_t message_body )
	{
		struct stat sb = p7::fstat( message_body );
		
		if ( !S_ISREG( sb.st_mode ) )
		{
			p7::throw_errno( ESPIPE );
		}
		
		return sb.st_size;
	}
	
	plus::string GetContentLengthLine( p7::fd_t message_body )
	{
		return HeaderFieldLine( "Content-Length", gear::inscribe_decimal( GetContentLength( message_body ) ) );
	}
	
	
	static inline HeaderFieldEntry MakeHeaderFieldEntry( std::size_t name,
	                                                     std::size_t colon,
	                                                     std::size_t value,
	                                                     std::size_t crlf )
	{
		HeaderFieldEntry result;
		
		result.field_offset = name;
		result.colon_offset = colon;
		result.value_offset = value;
		result.crlf_offset  = crlf;
		
		return result;
	}
	
	
	static HeaderIndex MakeHeaderIndex( const char* header_stream, const char* end )
	{
		HeaderIndex result;
		
		const char* p = header_stream;
		
		while ( p < end )
		{
			const char* field = p;
			
			const char* crlf = std::strstr( p, "\r\n" );
			
			if ( crlf == p )
			{
				// Empty line indicates end of header
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
			
			HeaderFieldEntry tuple = MakeHeaderFieldEntry( field - header_stream,
			                                               colon - header_stream,
			                                               value - header_stream,
			                                               crlf  - header_stream );
			
			result.push_back( tuple );
			
			p = crlf + STRLEN( "\r\n" );
		}
		
		// Ran out of data without finding end of header
		throw MalformedHeader();
	}
	
	static bool strings_case_insensitively_equal( const char* a, std::size_t a_len,
	                                              const char* b, std::size_t b_len )
	{
		if ( a_len != b_len )  return false;
		
		for ( const char* a_end = a + a_len;  a < a_end;  ++a, ++b )
		{
			if ( iota::to_lower( *a ) != iota::to_lower( *b ) )
			{
				return false;
			}
		}
		
		return true;
	}
	
	static const HeaderFieldEntry* FindHeaderFieldInStream( const char*         stream,
	                                                        const HeaderIndex&  index,
	                                                        const char*         name,
	                                                        std::size_t         name_length )
	{
		for ( HeaderIndex::const_iterator it = index.begin();  it != index.end();  ++it )
		{
			const char* field = stream + it->field_offset;
			
			std::size_t length = it->colon_offset - it->field_offset;
			
			if ( strings_case_insensitively_equal( field, length, name, name_length ) )
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
		// Are we receiving header or content?
		if ( !itHasReceivedEntireHeader )
		{
			// We haven't received the entire header yet.
			// We need to concatenate all received data to check for the EOH marker.
			itsReceivedData.append( data, byteCount );
			
			const char* begin = itsReceivedData.c_str();
			
			if ( itsStartOfHeaderFields == 0 )
			{
				if ( const char* crlf = strstr( begin, "\r\n" ) )
				{
					itsStartOfHeaderFields = crlf - begin + STRLEN( "\r\n" );
					
					itsPlaceToLookForEndOfHeader = crlf - begin;
				}
			}
			
			if ( itsPlaceToLookForEndOfHeader == 0 )
			{
				return;
			}
			
			const size_t received_data_size = itsReceivedData.size();
			
			if ( received_data_size - itsPlaceToLookForEndOfHeader < STRLEN( "\r\n" "\r\n" ) )
			{
				return;
			}
			
			const char* end_of_header = strstr( begin + itsPlaceToLookForEndOfHeader, "\r\n" "\r\n" );
			
			if ( end_of_header == NULL )
			{
				itsPlaceToLookForEndOfHeader = received_data_size - STRLEN( "\r\n" "\r\n" ) + 1;
				
				return;
			}
			
			// Found it, so we've got the entire header now
			itHasReceivedEntireHeader = true;
			
			// The content starts after the two CRLF
			const char* content = end_of_header + STRLEN( "\r\n" "\r\n" );
			
			const char* end = begin + received_data_size;
			
			// Anything left over is content
			const size_t leftOver = end - content;
			
			// Start writing content if we have any
			if ( leftOver > 0 )
			{
				ReceiveContent( content, leftOver );
				
				itsReceivedData.resize( content - begin );
			}
			
			const char* header_stream = GetHeaderStream();
			
			itsHeaderIndex = MakeHeaderIndex( header_stream, end );
			
			if ( const HeaderFieldEntry* contentLengthEntry = FindHeaderFieldInStream( header_stream,
			                                                                           itsHeaderIndex,
			                                                                           STR_LEN( "Content-Length" ) ) )
			{
				const char* contentLength = header_stream + contentLengthEntry->value_offset;
				
				// Now get the *real* value, as opposed to its textual representation
				itsContentLength = gear::parse_unsigned_decimal( contentLength );
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
		const std::size_t blockSize = 4096;
		
		char data[ blockSize ];
		
		std::size_t bytesToRead = blockSize;
		
		if ( itHasReceivedEntireHeader && itsContentLengthIsKnown )
		{
			std::size_t bytesToGo = itsContentLength - itsContentBytesReceived;
			
			if ( bytesToGo == 0 )
			{
				return false;
			}
			
			bytesToRead = std::min( bytesToRead, bytesToGo );
		}
		
		if ( int received = p7::read( socket, data, bytesToRead ) )
		{
			ReceiveData( data, received );
		}
		else
		{
			itHasReachedEndOfInput = true;
			
			if ( !itHasReceivedEntireHeader )
			{
				throw MalformedHeader();
			}
			
			if ( itsContentLengthIsKnown  &&  itsContentBytesReceived != itsContentLength )
			{
				throw IncompleteMessageBody();
			}
			
			return false;
		}
		
		return true;
	}
	
	void MessageReceiver::ReceiveHeader( p7::fd_t socket )
	{
		while ( !itHasReceivedEntireHeader && ReceiveBlock( socket ) )
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
	
	plus::string MessageReceiver::GetHeaderField( const plus::string& name, const char* nullValue )
	{
		const char* stream = GetHeaderStream();
		
		const HeaderFieldEntry* it = FindHeaderFieldInStream( stream,
		                                                      itsHeaderIndex,
		                                                      name.data(),
		                                                      name.size() );
		
		if ( it != NULL )
		{
			return plus::string( stream + it->value_offset,
				                 stream + it->crlf_offset );
		}
		
		if ( nullValue == NULL )
		{
			throw NoSuchHeaderField();
		}
		
		return nullValue;
	}
	
	
	plus::string ResponseReceiver::GetResult() const
	{
		const plus::string& message = GetMessageStream();
		
		if ( const char* p = std::strchr( message.data(), ' ' ) )
		{
			return plus::string( p + 1, GetHeaderStream() - 2 );
		}
		
		return "";
	}
	
	unsigned ResponseReceiver::GetResultCode() const
	{
		const plus::string& message = GetMessageStream();
		
		if ( const char* p = std::strchr( message.data(), ' ' ) )
		{
			return gear::parse_unsigned_decimal( p + 1 );
		}
		
		return 0;
	}
	
}
