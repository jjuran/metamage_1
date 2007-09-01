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

