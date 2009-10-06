/*	=======
 *	HTTP.hh
 *	=======
 */

// Standard C++
#include <string>
#include <vector>

// poseven
#include "poseven/FileDescriptor.hh"


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
			HeaderIndex itsHeaderIndex;
			std::string itsReceivedData;
			std::string itsPartialContent;
			std::size_t itsStartOfHeaderFields;
			std::size_t itsPlaceToLookForEndOfHeader;
			std::size_t itsContentLength;
			std::size_t itsContentBytesReceived;
			bool itHasReceivedEntireHeader;
			bool itsContentLengthIsKnown;
			bool itHasReachedEndOfInput;
			
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
			
			const std::string& GetMessageStream() const  { return itsReceivedData; }
			
			std::string GetStatusLine() const  { return itsReceivedData.substr( 0, itsStartOfHeaderFields - 2 ); }
			
			const char* GetHeaderStream() const  { return itsReceivedData.data() + itsStartOfHeaderFields; }
			
			const HeaderIndex& GetHeaderIndex() const  { return itsHeaderIndex; }
			
			std::string GetHeaderField( const std::string& name, const char* nullValue = NULL );
			
			const std::string& GetPartialContent() const  { return itsPartialContent; }
	};
	
	class ResponseReceiver : public MessageReceiver
	{
		public:
			std::string GetResult() const;
			
			unsigned GetResultCode() const;
	};
	
	
	inline std::string RequestLine( const std::string& method, const std::string& urlPath )
	{
		return method + " " + urlPath + " HTTP/1.0" "\r\n";
	}
	
	inline std::string HeaderFieldLine( const std::string& name, const std::string& value )
	{
		return name + ": " + value + "\r\n";
	}
	
	std::string GetContentLengthLine( poseven::fd_t message_body );
	
}

