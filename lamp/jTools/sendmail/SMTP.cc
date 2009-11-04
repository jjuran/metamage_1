/*	=======
 *	SMTP.cc
 *	=======
 */

#include "SMTP.hh"

// Standard C++
#include <algorithm>
#include <string>


namespace SMTP
{
	
	namespace Client
	{
		
		namespace p7 = poseven;
		
		
		struct ResponseCode
		{
			enum { size = 4 };
			
			ResponseCode( const std::string& response );
			
			char chars[ size ];
		};
		
		
		ResponseCode::ResponseCode( const std::string& response )
		{
			if ( response.size() < size )
			{
				throw InvalidResponse();
			}
			
			std::copy( response.begin(), response.begin() + size, chars );
		}
		
		static void GetResponse( text_input::feed feed, p7::fd_reader& reader )
		{
			while ( const std::string* s = get_line_from_feed( feed, reader ) )
			{
				ResponseCode code = *s;
				
				switch ( code.chars[ 3 ] )
				{
					default:
						throw InvalidResponse();
					
					case '-':
						continue;
					
					case ' ':
						switch ( code.chars[ 0 ] )
						{
							case '4': throw Error();
							case '5': throw Failure();
							
							default: return;
						}
				}
			}
			
			throw InvalidResponse();  // end of file -- connection dropped
		}
		
		void Session::verify_response()
		{
			GetResponse( its_feed, its_input_reader );
		}
		
	}  // namespace Client
	
}  // namespace SMTP

