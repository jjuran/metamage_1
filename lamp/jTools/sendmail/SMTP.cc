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
		
		
		ResponseCode::ResponseCode( const std::string& response )
		{
			if ( response.size() < size )
			{
				throw InvalidResponse();
			}
			
			std::copy( response.begin(), response.begin() + size, chars );
		}
		
		bool CheckResponse( const std::string& response )
		{
			if ( response.size() >= ResponseCode::size )
			{
				switch ( response[ 3 ] )
				{
					case ' ':  return true;
					case '-':  return false;
					
					default:  break;
				}
			}
			
			throw InvalidResponse();
		}
		
		ResponseCode GetResponse( text_input::feed feed, p7::fd_reader& reader )
		{
			while ( const std::string* s = get_line_from_feed( feed, reader ) )
			{
				std::string response = *s;
				
				ResponseCode code = response;
				
				if ( CheckResponse( response ) )
				{
					return code;
				}
				else
				{
					continue;
				}
			}
			
			throw InvalidResponse();
		}
		
		ResponseCode VerifySuccess( ResponseCode code )
		{
			if ( code.chars[ 0 ] == '4' )
			{
				throw Error();
			}
			
			if ( code.chars[ 0 ] == '5' )
			{
				throw Failure();
			}
			
			return code;
		}
		
	}  // namespace Client
	
}  // namespace SMTP

