/*	=======
 *	HTTP.hh
 *	=======
 */

// Standard C++
#include <vector>

// iota
#include "iota/strings.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/fd_t.hh"


namespace HTTP
{
	
	class MalformedHeader {};
	
	class IncompleteMessageBody {};
	
	class NoSuchHeaderField {};
	
	
	// Used to process an incoming message header
	struct HeaderFieldEntry
	{
		std::size_t field_offset;
		std::size_t colon_offset;
		std::size_t value_offset;
		std::size_t crlf_offset;
	};
	
	typedef std::vector< HeaderFieldEntry > HeaderIndex;
	
	
	class MessageReceiver
	{
		private:
			HeaderIndex       itsHeaderIndex;
			plus::var_string  itsReceivedData;
			plus::var_string  itsPartialContent;
			std::size_t       itsStartOfHeaderFields;
			std::size_t       itsPlaceToLookForEndOfHeader;
			std::size_t       itsContentLength;
			std::size_t       itsContentBytesReceived;
			bool              itHasReceivedEntireHeader;
			bool              itsContentLengthIsKnown;
			bool              itHasReachedEndOfInput;
			
			void ReceiveContent( const char* data, std::size_t byteCount );
			void ReceiveData   ( const char* data, std::size_t byteCount );
		
		public:
			MessageReceiver() : itsStartOfHeaderFields      ( 0 ),
			                    itsPlaceToLookForEndOfHeader( 0 ),
			                    itsContentLength            ( 0 ),
			                    itsContentBytesReceived     ( 0 ),
			                    itHasReceivedEntireHeader   ( false ),
			                    itsContentLengthIsKnown     ( false ),
			                    itHasReachedEndOfInput      ( false )
			{
			}
			
			bool ReceiveBlock( poseven::fd_t socket );
			
			void ReceiveHeader( poseven::fd_t socket );
			
			void Receive( poseven::fd_t socket );
			
			const plus::string& GetMessageStream() const  { return itsReceivedData; }
			
			plus::string GetStatusLine() const  { return itsReceivedData.substr( 0, itsStartOfHeaderFields - 2 ); }
			
			const char* GetHeaderStream() const  { return itsReceivedData.data() + itsStartOfHeaderFields; }
			
			const HeaderIndex& GetHeaderIndex() const  { return itsHeaderIndex; }
			
			plus::string GetHeaderField( const plus::string& name, const char* nullValue = NULL );
			
			const plus::string& GetPartialContent() const  { return itsPartialContent; }
	};
	
	class ResponseReceiver : public MessageReceiver
	{
		public:
			plus::string GetResult() const;
			
			unsigned GetResultCode() const;
	};
	
	
	inline plus::string RequestLine( const char* method, const plus::string& urlPath )
	{
		plus::var_string result = method;
		
		result += ' ';
		
		result.append( urlPath );
		
		result.append( STR_LEN( " HTTP/1.0" "\r\n" ) );
		
		return result;
	}
	
	inline plus::string HeaderFieldLine( const plus::string& name, const plus::string& value )
	{
		return name + ": " + value + "\r\n";
	}
	
	plus::string GetContentLengthLine( poseven::fd_t message_body );
	
}

