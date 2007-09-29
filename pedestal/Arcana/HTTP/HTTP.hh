/*	=======
 *	HTTP.hh
 *	=======
 */

// Standard C++
#include <string>

// POSeven
#include "POSeven/FileDescriptor.hh"


namespace HTTP
{
	
	class MalformedHeader {};
	
	class IncompleteMessageBody {};
	
	
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
			HeaderIndex itsHeaderIndex;
			std::string itsReceivedData;
			std::string itsPartialContent;
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
			MessageReceiver() : itsStartOfHeaders            ( 0 ),
			                    itsPlaceToLookForEndOfHeaders( 0 ),
			                    itsContentLength             ( 0 ),
			                    itsContentBytesReceived      ( 0 ),
			                    itHasReceivedAllHeaders      ( false ),
			                    itsContentLengthIsKnown      ( false ),
			                    itHasReachedEndOfInput       ( false )
			{
			}
			
			bool ReceiveBlock( poseven::fd_t socket );
			
			void ReceiveHeaders( poseven::fd_t socket );
			
			void Receive( poseven::fd_t socket );
			
			const std::string& GetMessageStream() const  { return itsReceivedData; }
			
			std::string GetStatusLine() const  { return itsReceivedData.substr( 0, itsStartOfHeaders - 2 ); }
			
			const HeaderIndex& GetHeaderIndex() const  { return itsHeaderIndex; }
			
			const char* GetHeaderStream() const  { return itsReceivedData.data() + itsStartOfHeaders; }
			
			const std::string& GetPartialContent() const  { return itsPartialContent; }
	};
	
	
	inline std::string RequestLine( const std::string& method, const std::string& urlPath )
	{
		return method + " " + urlPath + " HTTP/1.0" "\r\n";
	}
	
	inline std::string HeaderLine( const std::string& name, const std::string& value )
	{
		return name + ": " + value + "\r\n";
	}
	
	std::string GetContentLengthHeaderLine( poseven::fd_t message_body );
	
	inline void SendMessageHeader( poseven::fd_t       out,
	                               const std::string&  message_header )
	{
		poseven::write( out, message_header.data(), message_header.size() );
	}
	
	void SendMessageBody( poseven::fd_t  out,
	                      poseven::fd_t  message_body );
	
	inline void SendMessage( poseven::fd_t       out,
	                         const std::string&  message_header,
	                         poseven::fd_t       message_body )
	{
		SendMessageHeader( out, message_header );
		SendMessageBody  ( out, message_body   );
	}
	
	std::string ReceiveMessage( poseven::fd_t in,
	                            poseven::fd_t header_out,
	                            poseven::fd_t body_out );
	
}

