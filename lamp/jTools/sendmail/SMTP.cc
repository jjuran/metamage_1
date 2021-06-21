/*	=======
 *	SMTP.cc
 *	=======
 */

#include "SMTP.hh"

// more-libc
#include "more/string.h"


namespace SMTP
{
	
	namespace Client
	{
		
		namespace p7 = poseven;
		
		
		struct ResponseCode
		{
			enum { size = 4 };
			
			ResponseCode( const plus::string& response );
			
			char chars[ size ];
		};
		
		
		ResponseCode::ResponseCode( const plus::string& response )
		{
			if ( response.size() < size )
			{
				throw InvalidResponse();
			}
			
			mempcpy( chars, response.data(), size );
		}
		
		static void GetResponse( text_input::feed feed, p7::fd_reader& reader )
		{
			while ( const plus::string* s = get_line_bare_from_feed( feed, reader ) )
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
